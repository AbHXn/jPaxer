#include "parser.h"

PARSER_ERROR PARSER_ERR_RAISED		 = NO_PARSER_ERROR;
static short int stack_limit_counter = 0;
size_t line_number 					 = 1;

static inline void safe_push( char *str, size_t* index, char cchar, const size_t MAX_SIZE ){
		if ( *index < MAX_SIZE - 1 ) str[(*index)++] = cchar;
		else str[ *index ] = '\0';
}

static inline void terminate_string( char *str, const int index ){
	if( *( str  + index ) != '\0' ) *( str + index ) = '\0';
}

/* check if the list reading is completed else convert the index to string and return the suitable flag */
static inline void list_action ( WORKING_ENV** cur_node ){
	if( !cur_node || *cur_node == NULL ) return;

	int temp; 
	while( (temp = _getc( )) != EOF ){
		if( isspace( temp ) ){
			if( temp == '\n' ) ++line_number;
			continue;
		}
		break;
	}

	if( temp != COMMA )
		push_char_buffer( temp );
	if( temp == CLOSE_S ) return;

	char list_index_str[ MAX_KEY_DIGIT ];

	snprintf( list_index_str, MAX_KEY_DIGIT, "%d", (*cur_node)->list_index );	
	push_char_buffer( SEMI );
	push_char_buffer( APPO );
	push_string( list_index_str );
	
	(*cur_node)->FLAG = KEY_ENTERING ;
	(*cur_node)->list_index += 1;
}

/* Instead of passing key, value, ... etc just pass then ENV */
static inline WORKING_ENV* init_working_env( void ){
	WORKING_ENV* new_working_env = _malloc( sizeof( WORKING_ENV ) );
	if( !new_working_env ){
		fprintf(stderr,"new working env error\n");
		PARSER_ERR_RAISED = PARSER_ALLOCATION_ERROR;
		return NULL;
	}
	new_working_env->k_index	 = 0;
	new_working_env->v_index	 = 0;
	new_working_env->FLAG 	 	 = 0;
	new_working_env->list_index  = 0;
	return new_working_env;
}

/* reset after key value push job */
static inline void RESET_ENV ( WORKING_ENV** cur_env ){
	if( !cur_env || !(*cur_env) ) return;
	(*cur_env)->k_index   = 0;
	(*cur_env)->v_index   = 0;
	(*cur_env)->key[0] 	  = '\0';
	(*cur_env)->value[0]  = '\0';
}

static inline bool push_JSON_NODE_to_stack( JSON_NODE* j_node, STACK** stack ){
	STACK_DATA* new_stack_data = get_stack_data( j_node );
	if( !new_stack_data ) return false;
	if( stack_limit_counter == STACK_LIMIT ){
		ERROR(stderr, "JSON nested limit exeeded\n");
		PARSER_ERR_RAISED = PARSER_STACK_LIMIT_EXEEDED;
		return false;
	}
	++stack_limit_counter;
	return push_stack ( ( void* )new_stack_data,  stack );
}


static inline void terminate_value_if_filling( WORKING_ENV** env ){
	if( !env || !( *env ) ) return ;
	
	if ( (*env )->FLAG == VALUE_ENTERING ){
		( *env )->FLAG = KEY_VALUE_PAIR;
		terminate_string( ( *env )->value, ( *env )->v_index );
	}
}

/* STACK_DATA creation for push to stack */
STACK_DATA* get_stack_data( JSON_NODE* j_data ){
	if( !j_data ) return NULL;
	
	STACK_DATA* n_stack_data = _malloc( sizeof( STACK_DATA ) );
	if( !n_stack_data ){
		ERROR(stderr,"failed to create new parser error");
		PARSER_ERR_RAISED = PARSER_ALLOCATION_ERROR;
		return NULL;
	}
	n_stack_data->j_data	 = j_data;
	n_stack_data->list_index = 0;
	n_stack_data->FLAG 		 = 0;
	return n_stack_data;
}

/* a function to add the key value pair to object dtype */
bool add_J_NODE_to_jobject( JSON_NODE** j_node, JSON_NODE* push_node ){
	if( !j_node || !(*j_node) ){
		ERROR(stderr,"parser key value push error\n");
		PARSER_ERR_RAISED = PARSER_INTERNAL_ERROR;
		return false;
	}
	if( (*j_node)->dtype != J_OBJECT ){
		ERROR(stderr,"parser push dtype error\n");
		PARSER_ERR_RAISED = PARSER_INTERNAL_ERROR;
		return false;
	}
	jobject* n_jobj = get_jobject( push_node );
	if( !n_jobj ){ 
		ERROR(stderr, "%s\n", get_object_error_msg( get_object_error() ));
		PARSER_ERR_RAISED = PARSER_EXTERNAL_ERROR;
		return false;
	} 
	jobject* lists_objs = (*j_node)->value.object_val;
	if( !lists_objs ){
		(*j_node)->value.object_val = n_jobj;
		return true;
	}
	while( lists_objs->next )
		lists_objs = lists_objs->next;
	lists_objs->next = n_jobj;
	return true;
}

void add_parent_info_from_stack( JSON_NODE** new_node, STACK* dfs_stack ){
	if( !dfs_stack || !new_node || *new_node == NULL) 
		return;
	STACK_DATA* top_data = (STACK_DATA *) top( &dfs_stack );
	if( !top_data ) return;
	(*new_node)->parent = top_data->j_data;
}

JSON_NODE* get_full_JSON_NODE_pair_from_env( WORKING_ENV* env, bool is_string ){
	if( env->FLAG != KEY_VALUE_PAIR ){
		PARSER_ERR_RAISED = PARSER_SYNTAX_ERROR;
		return NULL;
	}
	JSON_DTYPE cur_value_type = J_SKIP;
	void* value_ptr			  = NULL;

	strip  ( env->key );
	strip  ( env->value );
	value_ptr = get_dtype( env->value, &cur_value_type );

	if( !value_ptr && cur_value_type == J_SKIP ){
		cur_value_type = non_returned_dtype( env->value );
		if( cur_value_type == J_STRING ){
			if ( is_string )
				value_ptr = get_J_STRING( env->value );
			else {
				PARSER_ERR_RAISED = PARSER_SYNTAX_ERROR;
				return NULL;
			}
		}
	}
	JSON_NODE* new_node = get_jnode( env->key );
	if( !new_node ){
		PARSER_ERR_RAISED = PARSER_ALLOCATION_ERROR;
		return NULL;
	}
	new_node->dtype = cur_value_type;
	fill_value_acc( &new_node, value_ptr, cur_value_type ); 
	return new_node;
}

bool FLUSH_THE_STACK( WORKING_ENV** env, STACK** dfs_stack ){
	if( !env || !dfs_stack ) return false;

	if( *env == NULL || *dfs_stack == NULL ) return false;

	WORKING_ENV* cur_env = *env;
	STACK_DATA* top_stack_data = ( STACK_DATA* ) top( dfs_stack );

	if( cur_env->FLAG >= VALUE_ENTERING ){
		bool is_string = !( cur_env->FLAG == VALUE_ENTERING );

		terminate_value_if_filling( env );

		JSON_NODE* new_pair = get_full_JSON_NODE_pair_from_env( cur_env, is_string );
		if( !new_pair ) return false;
				
		add_parent_info_from_stack( &new_pair, *dfs_stack );
		if( !add_J_NODE_to_jobject( &(top_stack_data->j_data), new_pair ) )
			return false;
	}
	if( !pop_stack( dfs_stack ) ) {
		ERROR( stderr, "%s\n", get_stack_error_msg( get_stack_error()) );
		PARSER_ERR_RAISED = PARSER_EXTERNAL_ERROR;
		return false;
	}
	--stack_limit_counter;
	// even if stack has one element return its flushed
	STACK_DATA* new_top_stack_data = ( STACK_DATA* ) top( dfs_stack );
	if( !new_top_stack_data ){
		push_stack( top_stack_data, dfs_stack );
		return true;
	}
	return add_J_NODE_to_jobject( &(new_top_stack_data->j_data), top_stack_data->j_data );
}

void handle_open_square_or_curly( WORKING_ENV** env, int c_char, STACK** dfs_stack  ){
	if( !env || !(*env) || !dfs_stack ) return;
	if( (*env)->FLAG == 0){
		PARSER_ERR_RAISED = PARSER_SYNTAX_ERROR;
		return;
	}
	strip( (*env)->key );
	JSON_NODE *new_node = get_jnode( (*env)->key );
	add_parent_info_from_stack( &new_node, *dfs_stack );

	if( !new_node ){
		ERROR( stderr, "%s\n", get_object_error_msg( get_object_error() ) );
		PARSER_ERR_RAISED = PARSER_ALLOCATION_ERROR;
		return;	
	}
	new_node->dtype 		   = J_OBJECT;
	new_node->multiple_values  = ( c_char == OPEN_S ) ? true: false; 
	new_node->value.object_val = NULL;

	if( *dfs_stack ){
		STACK_DATA* top_node = ( STACK_DATA* ) top( dfs_stack );
		top_node->list_index = (*env)->list_index;
		top_node->FLAG 		 = (*env)->FLAG;
	}
	if( !push_JSON_NODE_to_stack ( (void *) new_node, dfs_stack ) ){
		ERROR( stderr, "%s\n", get_stack_error_msg( get_stack_error() ) );
		PARSER_ERR_RAISED = PARSER_EXTERNAL_ERROR;
		return;
	}
	RESET_ENV ( env ); 
	( *env )->list_index = 0;
	if( c_char == OPEN_S )
		list_action( env );
	else (*env)->FLAG = 0;
}

void handle_comma( WORKING_ENV** env, int c_char, STACK** dfs_stack ){
	if( (*env)->FLAG < VALUE_ENTERING ){
		PARSER_ERR_RAISED = PARSER_SYNTAX_ERROR;
		return ;
	}	
	terminate_value_if_filling( env );

	JSON_NODE* new_node = get_full_JSON_NODE_pair_from_env( (*env), true );
	if( !new_node ) return ;

	add_parent_info_from_stack( &new_node, *dfs_stack );
	STACK_DATA* top_node = (STACK_DATA *) top( dfs_stack );
	if( !add_J_NODE_to_jobject( &(top_node->j_data), new_node ) )
		return ;

	RESET_ENV ( env ); 	
	if( (*env)->list_index > 0 )
		list_action( env );
	else (*env)->FLAG = 0;
}

void handle_closing_square_or_curly( WORKING_ENV**env, int c_char, STACK** dfs_stack ){
	if( !env || !(*env) || !dfs_stack ) return;
	
	if( !FLUSH_THE_STACK( env, dfs_stack ) ){
		ERROR(stderr, "Stack Flush Error\n");
		if( PARSER_ERR_RAISED == NO_PARSER_ERROR )
			PARSER_ERR_RAISED = PARSER_INTERNAL_ERROR;
		return ;
	}
	if( dfs_stack ){
		STACK_DATA* top_node = ( STACK_DATA* ) top( dfs_stack );
		RESET_ENV( env );
		(*env)->FLAG 		 = top_node->FLAG;
		(*env)->list_index   = top_node->list_index;
	}
	( *env )->FLAG = 0;

	if( (*env)->list_index > 0 )
		list_action( env );
}

void fill_inputs( WORKING_ENV** env, int c_char ){
	if( (*env)->FLAG == VALUE_LICENSE ) 
		(*env)->FLAG = VALUE_ENTERING;

	if( ( *env )->FLAG != KEY_ENTERING && ( *env )->FLAG != VALUE_ENTERING ){
		PARSER_ERR_RAISED = PARSER_SYNTAX_ERROR;
		return ;
	}
	( (*env)->FLAG == KEY_ENTERING ) \
		? safe_push( (*env)->key, &((*env)->k_index), c_char, MAX_KEY_SIZE )
		: safe_push( (*env)->value, &((*env)->v_index), c_char, MAX_VALUE_SIZE );
}

JSON_NODE* parse_JSON( READER* JSON_READER ){
	if( !JSON_READER ){
		ERROR(stderr, "Failed to Parse\n");
		return NULL;
	}

	int c_char;

	JSON_READER_OBJECT 			= JSON_READER;
	STACK* dfs_stack 			= NULL;
	FLUSH_TYPE flushed_before 	= NO_FLUSH_HAPPENDED;
	WORKING_ENV* env 			= init_working_env();
	bool string_flag			= false;
	
	if( !env ){
		PARSER_ERR_RAISED = PARSER_ALLOCATION_ERROR;
		return NULL;
	}

	strcpy( env->key, "DICT" );
	env->FLAG = KEY_ENTERING;

	while( ( c_char = _getc() ) != EOF ){
		if( c_char == '\n' ) ++line_number;

		if( is_json_syntax( c_char ) && c_char != APPO && !string_flag ){
			switch ( c_char ){
				case CLOSE_C: 
				case CLOSE_S:  {
					if( flushed_before == COMMA_FLUSH ){
						PARSER_ERR_RAISED = PARSER_SYNTAX_ERROR;
						goto END;
					}		
					handle_closing_square_or_curly( &env, c_char, &dfs_stack );
					if( PARSER_ERR_RAISED != NO_PARSER_ERROR )
						goto END;
					flushed_before = CLOSSING_FLUSH;
					break;
				}
				case OPEN_S: 
				case OPEN_C: 	
					handle_open_square_or_curly( &env, c_char, &dfs_stack );
					break;
				case SEMI: {
					if( env->FLAG == 0 || env->FLAG == VALUE_ENTERING  ){
						PARSER_ERR_RAISED = PARSER_SYNTAX_ERROR;
						goto END;
					}
					env->FLAG = VALUE_LICENSE;
					break;
				}
				case COMMA : { 
					if( flushed_before == CLOSSING_FLUSH ){
						flushed_before = COMMA_FLUSH;
						break;
					}else if( flushed_before == COMMA_FLUSH ){
						PARSER_ERR_RAISED = PARSER_SYNTAX_ERROR;
						goto END;
					}
					handle_comma( &env, c_char, &dfs_stack );
					if( PARSER_ERR_RAISED != NO_PARSER_ERROR )
						goto END;
					flushed_before = COMMA_FLUSH;
					break;
				}
			} // switch
			continue;
		}// if
		else if( c_char == APPO ){
			switch( env->FLAG ){
				case 0:
					string_flag = true;
					env->FLAG = KEY_ENTERING;
					continue;
				case KEY_ENTERING:
					string_flag = false;
					terminate_string( env->key, env->k_index );
					continue;
				case VALUE_LICENSE:
					env->FLAG = VALUE_ENTERING;
					string_flag = true;
					continue;
				case VALUE_ENTERING:
					string_flag = false;
					terminate_value_if_filling( &env );
					continue;
				default:
					terminate_value_if_filling( &env );
			}			
		}// else if
		else{
			if( isspace( c_char ) &&  env->FLAG != KEY_ENTERING && env->FLAG != VALUE_ENTERING ) continue;
			if( isspace( c_char ) && !string_flag ){
				terminate_value_if_filling( &env );	
				continue;
			}
			fill_inputs( &env, c_char );

			if( c_char == '\\' ){
				int temp = _getc();
				fill_inputs( &env, temp );
			}
			if( PARSER_ERR_RAISED != NO_PARSER_ERROR )
				goto END;
			
		}// else
		flushed_before = NO_FLUSH_HAPPENDED;	
	}
	END:
	if( PARSER_ERR_RAISED != NO_PARSER_ERROR ) {
		if( PARSER_ERR_RAISED == PARSER_SYNTAX_ERROR )
			PRINT_SYNTAX_ERROR( line_number, c_char );
		else ERROR(stderr, "Error occured..terminating..\n");
		free_memory();
		return NULL;
	}
	flush_buffer();
	return (JSON_NODE*) (((STACK_DATA* ) top( &dfs_stack ))->j_data);
}

JSON_NODE* parse_JSON_from_str( const char* json_str ){
	if( !json_str ){
		ERROR( stderr, "JSON string is NULL\n" );
		return NULL;
	}

	READER* new_reader_object = create_reader_for_str( json_str );
	if( !new_reader_object ){
		ERROR( stderr, get_read_error_msg( get_read_error() ) );
		return NULL;
	}

	return parse_JSON( new_reader_object ); 

}

JSON_NODE* parse_JSON_from_FILE( FILE* json_file ){
	if( !json_file ){
		ERROR( stderr, "JSON FILE is NULL\n" );
		return NULL;
	}

	READER* new_reader_object = create_reader_for_FILE( json_file );
	if( !new_reader_object ){
		ERROR( stderr, get_read_error_msg( get_read_error() ) );
		return NULL;
	}
	return parse_JSON( new_reader_object ); 
}
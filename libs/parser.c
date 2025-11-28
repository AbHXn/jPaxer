#include "parser.h"


/*
	implemeted many flags for minute error checking but not implemented properly
*/

PARSER_ERROR PARSER_ERR_RAISED = NO_PARSER_ERROR;
static short int stack_limit_counter = 0;

static inline bool is_value_filling( int _flag ){
	return !is_value_filled ( _flag ) && is_in_filling_mode	( _flag );
}

static inline bool is_key_value_pair( int _flag ){	
	return is_key_filled( _flag ) && is_value_filled( _flag ) && !is_in_filling_mode( _flag );
}

static inline void safe_push( char *str, size_t* index, char cchar, const size_t MAX_SIZE ){
		if ( *index < MAX_SIZE - 1 ) str[(*index)++] = cchar;
		else str[ *index ] = '\0';
}

static inline void terminate_string( char *str, const int index ){
	if( *( str  + index ) != '\0' ) *( str + index ) = '\0';
}

static inline int get_next_not_space( void ){
	int temp; while( isspace(temp = _getc( )) ); return temp;
}

/* check if the list reading is completed else convert the index to string and return the suitable flag */
static inline void list_action ( WORKING_ENV** cur_node ){
	if( !cur_node || *cur_node == NULL ) return;

	int temp = get_next_not_space();
	push_char_buffer( temp );
	if( temp == CLOSE_S ) return;

	char list_index_str[ MAX_KEY_DIGIT ];

	snprintf( list_index_str, MAX_KEY_SIZE, "%d", (*cur_node)->list_index );	
	push_char_buffer( SEMI );
	push_char_buffer( APPO );
	push_string( list_index_str );
	
	(*cur_node)->FLAG = 0b00000 | SOMETHING_INPUTING | LIST_INPUTING | STRING_INPUTING ;
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
	new_working_env->FLAG 	 	 = 0b00000;
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

static inline bool is_json_syntax( char c ){
	switch( c ) {
		case OPEN_C	: case CLOSE_C	:
		case OPEN_S	: case APPO		:
		case SEMI	: case CLOSE_S	:
		case COMMA	:
			return true;
		default:
			return false;
	}
}

/* STACK_DATA creation for push to stack */
STACK_DATA* get_stack_data( JSON_NODE* j_data ){
	if( !j_data ) return NULL;
	
	STACK_DATA* n_stack_data = _malloc( sizeof( STACK_DATA ) );
	if( !n_stack_data ){
		ERROR(stderr,"failed to create new parser error");
		PARSER_ERR_RAISED = NO_PARSER_ERROR;
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

JSON_NODE* get_full_JSON_NODE_pair_from_env( WORKING_ENV* env ){
	if( !is_key_value_pair( env->FLAG ) ){
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
		if( cur_value_type == J_STRING )
			value_ptr = get_string( env->value );
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

	if( is_key_value_pair( cur_env->FLAG ) ){
		JSON_NODE* new_pair = get_full_JSON_NODE_pair_from_env( cur_env );
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

JSON_NODE* parse_JSON_from_FILE( FILE* JSON_file ){
	if( !JSON_file ){
		ERROR(stderr, "JSON file not provide\n");
		return NULL;
	}
	
	JSON_FILE = JSON_file;
	int    c_char;
	STACK* dfs_stack 		= NULL;
	bool   flushed_before   = false;

	WORKING_ENV* env = init_working_env();
	
	if( !env ){
		PARSER_ERR_RAISED = PARSER_ALLOCATION_ERROR;
		return NULL;
	}
	strcpy( env->key, "DICT" );
	env->FLAG = turn_on( env->FLAG, KEY_ENTERED );
	size_t line_number = 1;

	while( ( c_char = _getc() ) != EOF ){
		if( c_char == '\n' ) ++line_number;
		
		if( is_json_syntax( c_char ) && !is_string_inputing( env->FLAG ) && c_char != APPO ){
			switch ( c_char ){
				case CLOSE_C: 
				case CLOSE_S:  {
					if( is_in_filling_mode( env->FLAG ) ){
						env->FLAG = turn_off( env->FLAG, SOMETHING_INPUTING );
						env->FLAG = turn_on( env->FLAG, VALUE_ENTERED );
						terminate_string( env->value, env->v_index );
					}
					if( !FLUSH_THE_STACK( &env, &dfs_stack ) ){
						ERROR(stderr, "Stack Flush Error\n");
						PARSER_ERR_RAISED = PARSER_INTERNAL_ERROR;
						break;
					}
					if( dfs_stack != NULL ){
						STACK_DATA* top_node = ( STACK_DATA* ) top( &dfs_stack );
						RESET_ENV( &env );
						env->FLAG = top_node->FLAG;
						env->list_index  =  top_node->list_index;
					}
					env->FLAG &= LIST_INPUTING;
					
					if( is_list_filling( env->FLAG ) )
						list_action( &env );
					flushed_before = true;
					break;
				}
				case OPEN_S:
				case OPEN_C: {	
					if( !is_key_filled( env->FLAG ) && !is_in_filling_mode( env->FLAG )){
						PARSER_ERR_RAISED = PARSER_SYNTAX_ERROR;
						goto END;
					}
					strip( env->key );
					JSON_NODE *new_node = get_jnode( env->key );
					add_parent_info_from_stack( &new_node, dfs_stack );

					if( !new_node ){
						ERROR( stderr, "%s\n", get_object_error_msg( get_object_error() ) );
						PARSER_ERR_RAISED = PARSER_ALLOCATION_ERROR;
						goto END;	
					}
					new_node->dtype 		   = J_OBJECT;
					new_node->multiple_values  = ( c_char == OPEN_S ) ? true: false; 
					new_node->value.object_val = NULL;

					if( dfs_stack != NULL ){
						STACK_DATA* top_node = ( STACK_DATA* ) top( &dfs_stack );
						top_node->list_index = env->list_index;
						top_node->FLAG 		 = env->FLAG;
					}
					if( !push_JSON_NODE_to_stack ( (void *) new_node, &dfs_stack ) ){
						ERROR( stderr, "%s\n", get_stack_error_msg( get_stack_error() ) );
						PARSER_ERR_RAISED = PARSER_EXTERNAL_ERROR;
						goto END;
					}
					RESET_ENV ( &env ); 
					if( c_char == OPEN_S ){
						env->list_index = 0;
						list_action( &env );
					}
					else env->FLAG = 0b00000;
					break;
				}
				case SEMI: {
					if( !is_key_filled ( env->FLAG ) || is_in_filling_mode( env->FLAG )){
						PARSER_ERR_RAISED = PARSER_SYNTAX_ERROR;
						goto END;
					}
					env->FLAG = turn_on( env->FLAG, VALUE_LICENSE );
					break;
				}
				case COMMA : {  
					if( flushed_before ){ flushed_before = false; break; }

					if( is_value_filling(env->FLAG) ){
						env->FLAG = turn_off( env->FLAG, SOMETHING_INPUTING );
						env->FLAG = turn_on( env->FLAG, VALUE_ENTERED );
						terminate_string( env->value, env->v_index );
					}
					JSON_NODE* new_node = get_full_JSON_NODE_pair_from_env( env );
					add_parent_info_from_stack( &new_node, dfs_stack );
					if( !new_node ) break;

					STACK_DATA* top_node = (STACK_DATA *) top( &dfs_stack );
					if( !add_J_NODE_to_jobject( &(top_node->j_data), new_node ) )
						goto END;

					RESET_ENV ( &env ); 	
					if( is_list_filling( env->FLAG ) )
						list_action( &env );
					else env->FLAG = 0b00000;
					break;
				}
			} // switch
		}// if
		else if( c_char == APPO ){
			if( !is_in_filling_mode( env->FLAG ) ){
				if( is_value_licensed( env->FLAG ) )
					env->FLAG = turn_off( env->FLAG, VALUE_LICENSE );
				env->FLAG = turn_on( env->FLAG, SOMETHING_INPUTING | STRING_INPUTING  );
			}
			else{
				if( !is_key_filled( env->FLAG ) ){
					env->FLAG = turn_off(env->FLAG, SOMETHING_INPUTING | STRING_INPUTING);
					env->FLAG = turn_on( env->FLAG, KEY_ENTERED );
					terminate_string( env->key, env->k_index );
				}
				else{
					env->FLAG = turn_on ( env->FLAG, VALUE_ENTERED );
					env->FLAG = turn_off( env->FLAG, SOMETHING_INPUTING | STRING_INPUTING );
					terminate_string( env->value, env->v_index );
				}
			}
		}// else if
		else{
			if( isspace( c_char ) && !is_in_filling_mode( env->FLAG )) continue;
			if( isspace( c_char ) && is_in_filling_mode( env->FLAG ) 
								  && !is_string_inputing( env->FLAG )){
				env->FLAG = turn_off( env->FLAG, SOMETHING_INPUTING );
				env->FLAG = turn_on( env->FLAG, VALUE_ENTERED );
				terminate_string( env->value, env->v_index );
				continue;
			}
			if( c_char == '\\' ) c_char = _getc();

			if( is_value_licensed( env->FLAG ) ){
				env->FLAG = turn_off( env->FLAG, VALUE_LICENSE );
				env->FLAG = turn_on( env->FLAG, SOMETHING_INPUTING );
			}
			if( !is_in_filling_mode( env->FLAG ) ){
				PARSER_ERR_RAISED = PARSER_SYNTAX_ERROR;
				goto END;
			}
			( !is_key_filled( env->FLAG ) ) \
				? safe_push( env->key, &(env->k_index), c_char, MAX_KEY_SIZE )
				: safe_push( env->value, &(env->v_index), c_char, MAX_VALUE_SIZE );
			
		}// else	
	}
	END:
	if( PARSER_ERR_RAISED != NO_PARSER_ERROR) {
		if( PARSER_ERR_RAISED == PARSER_SYNTAX_ERROR ){
			int *ptr = _malloc( sizeof( int ) );
			if( ptr == NULL ){
				ERROR(stderr, "For errors error occured\n");
				return NULL;
			}
			const char* err_string = get_string_some_range( 10, ptr);
			SYNTAX_ERROR ( line_number );
			printf("\n\033[1;31m%s\033[0m\n", err_string);
			while( --(*ptr) > 0 ) putchar(' ');
			puts("^\n");
		}
		else ERROR(stderr, "Error occured..terminating..\n");
		free_memory();
		return NULL;
	}
	return (JSON_NODE*) (((STACK_DATA* ) top( &dfs_stack ))->j_data);
}
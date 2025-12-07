#include "jpaxer.h"

void write_escaped_string(FILE *json_file, const char *str) {
    fputc('"', json_file);  // start string

    for ( ; *str; str++ ) {
        switch ( *str ) {
            case '"':  
                fputs("\\\"", json_file); 
                break;

            case '\\': 
                fputs("\\\\",json_file); 
                break;

            case '\n': 
                fputs("\\n", json_file); 
                break;

            case '\r': 
                fputs("\\r", json_file); 
                break;

            case '\t': 
                fputs("\\t", json_file); 
                break;

            case '\b': 
                fputs("\\b", json_file); 
                break;

            case '\f': 
                fputs("\\f", json_file); 
                break;

            default:
                if ( (unsigned char)*str < 0x20 )   
                    fprintf(json_file, "\\u%04x", (unsigned char)*str);
                else
                    fputc(*str, json_file); 
        }
    }
    fputc('"', json_file); 
}

 void print_node(JSON_NODE* node, int level) {
    for (int i = 0; i < level; i++) printf("  "); // indentation
    
    printf(C_KEY "\"%s\"" C_RESET ": ", node->key);
    
    switch (node->dtype) 
    {
        case J_STRING:
            printf(C_STRING "\"%s\"" C_RESET, node->value.string_val);
            break;

        case J_INT:
            printf(C_NUMBER "%ld" C_RESET, *(node->value.int_val));
            break;

        case J_DOUBLE:
            printf(C_NUMBER "%lf" C_RESET, *(node->value.double_val));
            break;

        case J_BOOL:
            printf(C_BOOL "%s" C_RESET,
                   *(node->value.bool_val) ? "true" : "false");
            break;

        case J_NULL:
            printf(C_NULL "null" C_RESET);
            break;

        case J_OBJECT:
            printf(C_BRACE "{\n" C_RESET);
            
            jobject* cur = node->value.object_val;
            
            while (cur) 
            {
                print_node(cur->j_node, level + 1);

                if (cur->next) printf(",\n"); else printf("\n");
                
                cur = cur->next;
            }
            
            for (int i = 0; i < level; i++) printf("  ");
            
            printf(C_BRACE "}" C_RESET);
            
            break;
        default:
            printf("(unknown)");
    }
}

void print_JSON_node(JSON_NODE* root) {
    if (!root) return;

    printf(C_BRACE "{\n" C_RESET);

    if( root->dtype != J_OBJECT )
    {
        print_node( root, 1 );
        printf("\n");
    }
    else{
        jobject* cur = root->value.object_val;

        while (cur) 
        {
            print_node(cur->j_node, 1);

            if (cur->next) printf(",\n"); else printf("\n");
            
            cur = cur->next;
        }
    }
    printf(C_BRACE "}\n" C_RESET);
}

void JSON_FLUSH(JSON_NODE* json_data, FILE* json_file, int indent) {
    if (!json_data) return;

    int is_list       = json_data->multiple_values;
    jobject* contents = json_data->value.object_val;
    bool first        = true;

    for (int i = 0; i < indent; i++) 
        fputc(' ', json_file);

    fputc(is_list ? '[' : '{', json_file);
    fputc('\n', json_file);

    while (contents) {
        JSON_NODE* jnode = contents->j_node;

        if (!first) {
            fputc(',', json_file);
            fputc('\n', json_file);
        }
        first = false;

        for ( int i = 0; i < indent + 4; i++ ) 
            fputc(' ', json_file);

        if ( !is_list ) {
            write_escaped_string( json_file, jnode->key );
            fputs(": ", json_file);
        }


        switch (jnode->dtype) {
            case J_STRING:
                write_escaped_string( json_file, jnode->value.string_val );
                break;

            case J_INT:
                fprintf(json_file, "%ld", *(jnode->value.int_val));
                break;

            case J_DOUBLE:
                fprintf(json_file, "%f", *(jnode->value.double_val));
                break;

            case J_BOOL:
                fprintf(json_file, "%s", *(jnode->value.bool_val) ? "true" : "false");
                break;

            case J_NULL:
                fprintf(json_file, "null");
                break;

            case J_OBJECT:
                fputc('\n', json_file);
                JSON_FLUSH(jnode, json_file, indent + 4);
                break;
        }
        contents = contents->next;
    }
    fputc('\n', json_file);
    
    for (int i = 0; i < indent; i++) 
        fputc(' ', json_file);

    fputc(is_list ? ']' : '}', json_file);
}

void __read__( JSON_NODE** j_node, int counts, ... ){
    if( j_node == NULL || *j_node == NULL ) return ;
    
    va_list args;
    va_start( args, counts );

    for( int cnt = 0; cnt < counts; cnt++ )
    {
        const char* key = va_arg( args, const char* );
        JSON_NODE* c_jnode = *j_node;

        if( c_jnode->dtype != J_OBJECT )
        {
            ERROR(stderr, "JSON_NODE is not a object to get key\n" );
            return;
        }  

        jobject* pairs = c_jnode->value.object_val;

        while( pairs )
        {
            if( strcmp( pairs->j_node->key, key ) == 0 )
            {
                *j_node = pairs->j_node;
                break;
            }
            pairs = pairs->next;
        }

        if( pairs == NULL ) 
            ERROR(stderr, "Object has no key %s\n", key );
    }
}

void __update__( JSON_NODE** j_node, const char* key, void* value, JSON_DTYPE dtype ){
    if( j_node == NULL || *j_node == NULL || !key || !value ) return ;
    
    JSON_NODE* c_jnode = *j_node;
    
    if( c_jnode->dtype != J_OBJECT )
    {
        ERROR(stderr, "JSON_NODE is not an object get key\n");
        return ;
    }

    jobject* pairs = c_jnode->value.object_val;
    while( pairs ) 
    {
        if( strcmp( pairs->j_node->key, key ) == 0 )
        {
            JSON_NODE* obj = pairs->j_node;
            // its a stack memory
            switch( obj->dtype )
            {
                case J_INT:
                    obj->value.int_val    = NULL;
                    break;

                case J_DOUBLE:
                    obj->value.double_val = NULL;
                    break;

                case J_STRING:
                    obj->value.string_val = NULL;
                    break;

                case J_BOOL:
                    obj->value.bool_val   = NULL;
                    break;

                case J_OBJECT:
                    obj->value.object_val = NULL;
                    break;

                default:
                    break;
            }

            fill_value_acc( &obj, value, dtype );
            obj->dtype = dtype;
            return;
        }
        pairs = pairs->next;
    }
    ERROR(stderr, "key: %s doesn't exists\n", key );
}

void __delete__( JSON_NODE** j_node, const char* key ){
    if( j_node == NULL || *j_node == NULL || !key ) 
        return ;

    JSON_NODE* c_jnode = *j_node;
    
    if( c_jnode->dtype != J_OBJECT )
    {
        ERROR(stderr, "JSON_NODE is not an object get key\n");
        return ;
    }

    jobject* pairs = c_jnode->value.object_val;
    jobject* prev  = NULL;

    while( pairs ){
        if( pairs->j_node && strcmp( pairs->j_node->key, key ) == 0 )
        {
            
            if( pairs->next )
                pairs->next->prev = pairs->prev;

            if( prev )
                prev->next = pairs->next;
            else 
                c_jnode->value.object_val = pairs->next;
    
            return;
        }

        prev = pairs;
        pairs = pairs->next;
    }
    ERROR( stderr, "key: %s doesn't exists\n" , key );
}

void __create__( JSON_NODE** j_node, const char* key, void* value, JSON_DTYPE dtype ){
    if( j_node == NULL || *j_node == NULL || !key || !value )  return ;
    
    JSON_NODE* c_jnode = *j_node;
    
    if( c_jnode->dtype != J_OBJECT )
    {
        ERROR(stderr, "JSON_NODE is not an object get key\n");
        return ;
    }
    
    jobject* pairs    = c_jnode->value.object_val;
    JSON_NODE* n_node = get_jnode( key );

    if( !n_node )
    {
        ERROR(stderr, "Failed to create new node\n");
        return ;
    }

    fill_value_acc( &n_node, value, dtype );

    n_node->dtype  = dtype;
    
    add_J_NODE_to_jobject( j_node, n_node );
}

void __back__( JSON_NODE** j_node, int steps ){
    if( !j_node || *j_node == NULL ) return;
    while( steps-- > 0 && (*j_node)->parent != NULL )
        *j_node = (*j_node)->parent;
}


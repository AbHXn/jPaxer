#include <stdio.h>
#include "libs/jpaxer.h"

int main( int argc, char *argv[] ){
	FILE* jfile = fopen( "test_jsons/test1.json", "r" );

	if( !jfile ){
		fprintf(stderr, "Failed to open json file");
		return -1;
	}

	JSON_NODE* p_json = parse_JSON_from_FILE( jfile );
	get( p_json, 3, "configurations", "0", "params");

	print_JSON_node( p_json );

	JSON_NODE* new_node = get_jnode( "admin_info" );
	if( !new_node ){
		fprintf(stderr, "failed to create new jnode");
		return -1;
	}
	new_node->dtype = J_OBJECT;
	
	int* score = malloc(sizeof(int));
	if( !score ){
		fprintf(stderr, "failed to allcate int\n");
		return -1;
	}
	*score = 13405;

	add( new_node, "admin score", (void *)score, J_INT );
	print_JSON_node( new_node ); 

	// FILE* test = fopen( "final.json", "w");
	// JSON_FLUSH( p_json, test, 1 );
	return 0;
}
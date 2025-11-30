#include <stdio.h>
#include "libs/jpaxer.h"

int main( int argc, char *argv[] ){
	FILE* jfile = fopen( argv[1], "r" );

	if( !jfile ){
		fprintf(stderr, "Failed to open json file");
		return -1;
	}

	JSON_NODE* p_json = parse_JSON_from_FILE( jfile );

	print_JSON_node( p_json );

	// FILE* jile = fopen( "TEST.json", "w" );
	// JSON_FLUSH( p_json, jile, 2 );

	return 0;
}
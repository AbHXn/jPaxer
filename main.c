#include <stdio.h>
#include "libs/jpaxer.h"

int main( int argc, char *argv[] ){
	FILE* jfile = fopen( argv[1], "r" );
	JSON_NODE* p_json = parse_JSON_from_FILE( jfile );
	get( p_json, "configurations" );
	get( p_json, "0" );
	del( p_json, "id" );
	back(p_json);
	back(p_json);
	print_JSON_node( p_json );
	FILE* test = fopen( "final.json", "w");
	JSON_FLUSH( p_json, test, 1 );
	return 0;
}
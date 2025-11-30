#include <stdio.h>
#include "libs/jpaxer.h"

int main( int argc, char *argv[] ){
	FILE* jfile1 = fopen( "test_jsons/test1.json", "r" );

	if( !jfile1 ){
		fprintf(stderr, "Failed to open json file");
		return -1;
	}

	JSON_NODE* first_json = parse_JSON_from_FILE( jfile1 );
	fclose( jfile1 );

	FILE* jfile2 = fopen( "test_jsons/test2.json", "r" );

	if( !jfile2 ){
		fprintf(stderr, "Failed to open json file");
		return -1;
	}

	JSON_NODE* second_json = parse_JSON_from_FILE( jfile2 );
	fclose( jfile2 );

	get( first_json, 2, "configurations", "0" );
	get( second_json, 2, "0", "teamMembers" );

	print_JSON_node( first_json );
	print_JSON_node( second_json );

	set( first_json, "params", ( void* )second_json, J_OBJECT );

	back( second_json, 1 );

	get( second_json, 1, "details" );
	add( first_json, "new_details", ( void* )second_json, J_OBJECT );
	add( first_json, "whoisthis", get_J_STRING("admin"), J_STRING );

	add( first_json, "user_id", get_J_INT(123), J_INT );

	print_JSON_node( first_json );

	FILE* jile = fopen( "TEST.json", "w" );
	JSON_FLUSH( first_json, jile, 2 );

	return 0;
}
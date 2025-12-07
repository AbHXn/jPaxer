#include <stdio.h>
#include "jpaxer.h"

int main( int argc, char *argv[] ){
	// A simple script for modifying json 
	const char *json_str =
		"{"
		  "\"user\": {"
		    "\"id\": 12345,"
		    "\"name\": \"Alice \\\"The Dev\\\" Smith\","
		    "\"email\": \"alice@example.com\","
		    "\"active\": true,"
		    "\"roles\": [\"admin\", \"editor\", \"tester\"],"
		    "\"profile\": {"
		      "\"age\": 29,"
		      "\"height\": 165.5,"
		      "\"preferences\": {"
		        "\"theme\": \"dark\","
		        "\"notifications\": {"
		          "\"email\": true,"
		          "\"sms\": false"
		        "}"
		      "}"
		    "}"
		  "},"
		  "\"projects\": ["
		    "{"
		      "\"id\": \"proj_001\","
		      "\"name\": \"Malware Analyzer\","
		      "\"tags\": [\"c\", \"security\", \"parser\"],"
		      "\"stats\": {"
		        "\"stars\": 418,"
		        "\"forks\": 73,"
		        "\"active\": true"
		      "}"
		    "},"
		    "{"
		      "\"id\": \"proj_002\","
		      "\"name\": \"AI Engine\","
		      "\"tags\": [\"ml\", \"dl\", \"transformer\"],"
		      "\"stats\": {"
		        "\"stars\": 1024,"
		        "\"forks\": 256,"
		        "\"active\": false"
		      "}"
		    "}"
		  "],"
		  "\"misc\": {"
		    "\"null_field\": null,"
		    "\"escaped\": \"line1\\nline2\\t✓ Unicode\","
		    "\"values\": [1, 2, 3, 4.75, -10, 6.02e23]"
		  "}"
		"}";

	

	JSON_NODE* first_json = parse_JSON_from_str( json_str );

	FILE* jfile2 = fopen( "jfiles/test2.json", "r" );

	if( !jfile2 ){
		fprintf(stderr, "Failed to open json file");
		return -1;
	}

	JSON_NODE* second_json = parse_JSON_from_FILE( jfile2 );
	fclose( jfile2 );

	// second_json -> json node
	// 2 -> more 2 keys
	// 0 -> key 1 
	// "tempMembers" -> key 2  
	get( second_json, 2, "0", "teamMembers" );

	print_JSON_NODE( first_json );
	print_JSON_NODE( second_json );

	add( first_json, "params", ( void* )second_json, J_OBJECT );

	print_JSON_NODE( first_json );

	FILE* new_jfile = fopen( "result.json", "w" );
	save_JSON_to_FILE( first_json, new_jfile, 2 );
	fclose( new_jfile );

	free_memory();

	return 0;
}
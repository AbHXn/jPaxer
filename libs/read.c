#include "read.h"
#include "error.h"

READ_ERRORS READ_ERR_RAISED  = NO_READ_ERRORS;
FILE* JSON_FILE = NULL;
const char* _READ_ERR_TYPE = "Read Error: ";

unsigned char 	file_reader_buffer[MAX_READ_SIZE + SAFE_SIZE];
unsigned char* 	counter 	= NULL;

size_t 	byte_read 	= 0;
bool 	HIT_END 	= false;

const char* get_read_error_msg( READ_ERRORS err_raised ){
	if ( err_raised >= NO_READ_ERRORS && err_raised < READ_ERR_ENDS ) {
        const char *msg = read_error_msgs[ err_raised ];
        const char *prefix = _READ_ERR_TYPE;
       	return get_full_error_msg( prefix, msg );
    }
    return NULL;
}

void strip(char *s) {
    char *end;
    if (s == NULL || *s == '\0')
        return;
    while (isspace((unsigned char)*s)) 
        s++;
    if (*s == '\0') {
        *s = '\0';
        return;
    }
    end = s + strlen(s) - 1;
    while (end > s && isspace((unsigned char)*end)) 
        end--;
    *(end + 1) = '\0';
}

READ_ERRORS get_read_error( void ){
	READ_ERRORS err_raised = READ_ERR_RAISED;
	READ_ERR_RAISED = NO_READ_ERRORS;
	return err_raised;
}

char* get_string_some_range( size_t range, int *error_ptr ){
	if( !counter || byte_read == 0 || !error_ptr )
		return NULL;

	ptrdiff_t total_read_left = counter - file_reader_buffer;

	if( total_read_left > range )
		total_read_left = range;

	*error_ptr = 0;
	size_t idx_ctr = 0;
	
	char* error_found_string = _malloc( total_read_left + 1 );
	unsigned char* start = counter - total_read_left;

	while( start != counter ){
		if ( *start == '\n' )
			*error_ptr = 0;
		else (*error_ptr)++;
		error_found_string[ idx_ctr++ ] = *start++;
	}

	error_found_string[idx_ctr] = '\0';
	return error_found_string;
}

bool push_char_buffer( const char s ){
	if( counter == file_reader_buffer ){
		READ_ERR_RAISED = READ_READER_UNDERFLOW;
		return false;
	}
	*--counter = s;
	return true;
}

bool push_string( const char* str ){
	if( !counter ){
		READ_ERR_RAISED = READ_COUNTER_FAILED;
		return false;
	}
	if( !str ){
		READ_ERR_RAISED = READ_PUSH_STRING_NULL;
		return false;
	}
	const size_t str_size = strlen( str );
	const char*	temp_ptr  = str;
	ptrdiff_t avai_size   = counter - file_reader_buffer;

	if( str_size == 0 ) return true;

	if(	avai_size < ( ptrdiff_t ) str_size ){
		READ_ERR_RAISED = READ_READER_OVERFLOW;
		return false;
	}
	str += str_size - 1;
	while( str != temp_ptr )
		*--counter = *str--;
	*--counter = *str;
	return true;
}

bool reverse_pointer( size_t reverse_size ) {
	if( !counter ){
		READ_ERR_RAISED = READ_COUNTER_FAILED;
		return false;
	}
	ptrdiff_t avai_size = counter - file_reader_buffer;
	if( avai_size < ( ptrdiff_t) reverse_size ){
		READ_ERR_RAISED = READ_READER_OVERFLOW;
		return false;
	}
	counter -= reverse_size;
	return true;
}

void load_next_buffer( size_t size ) {
	if( !JSON_FILE ){
		READ_ERR_RAISED = READ_NULL_FILE_ERROR;
		return;
	}
	counter = file_reader_buffer + SAFE_SIZE;
	size_t read_size = ( size != 0 ) ? size: MAX_READ_SIZE;
	byte_read = fread( file_reader_buffer + SAFE_SIZE, 1, read_size, JSON_FILE );

	if( byte_read == 0 && ferror(JSON_FILE) ){
		READ_ERR_RAISED = READ_FILE_READ_ERROR;
	}
	HIT_END = feof( JSON_FILE );
}

int _getc( void ){
	if( byte_read == 0 )
		load_next_buffer( 0 );

	if( byte_read == 0 ) return EOF;

	unsigned char* max_addr = file_reader_buffer + byte_read + SAFE_SIZE;
	if( counter < max_addr )
		return (unsigned char) *counter++;
	else{
		if( !HIT_END ){
			load_next_buffer( 0 );
			if( byte_read > 0 && READ_ERR_RAISED == NO_READ_ERRORS )
				return (unsigned char) *counter++;
		}
	}
	return EOF;
}

void _ungetc(void) {
    if(counter > file_reader_buffer)
        counter--;
}
#ifndef ALLOC_H
#define ALLOC_H

#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

const char* get_full_error_msg( const char *msg,  const char *prefix );

#define _EACH_BLOCK_SIZE (1024 * 2024)
typedef struct memory _memory;

extern _memory* _mem_head;

typedef struct memory{
	unsigned char*  mem;
	size_t 			curr_alloc;
	struct memory* next_stack;
}_memory;

typedef enum _alloc_errors{
	NO_ALLOCATION_ERRORS   ,
	ALLOCATION_MEMORY_FAILED,
	ALLOCATION_BLOCK_FAILED ,
	ALLOCATION_LIMIT_EXEEDED,
	ALLOCATION_ERR_ENDS		,
}ALLOCATION_ERRORS;

extern const char* _ALLOC_ERR_TYPE;
extern ALLOCATION_ERRORS ALLOCATION_ERR_RAISED ;

const char* get_allocation_err_msg( ALLOCATION_ERRORS err );
ALLOCATION_ERRORS get_allocation_err( void );

static const char* allocation_error_msgs[] = {
	"no allocation errors occured\n",
	"struct memory allocation failed\n",
	"block allocation failed in memory\n",
	"requested allocation size exeeded its limit\n"
};

bool 	alloc_next		( void );
void* 	_malloc			( size_t alloc_size );
void 	free_memory	 	( void );
size_t  get_total_arena_allocated( void );

#endif
#include "alloc.h"

/* CUSTOM FUNCTION FOR MALLOC IN C FOR FASTER ALLOCATION */

static size_t total_arena_allocated = 0;

_memory* _mem_head = NULL;
const char* _ALLOC_ERR_TYPE = "Allocation Error: ";
ALLOCATION_ERRORS ALLOCATION_ERR_RAISED = NO_ALLOCATION_ERRORS;

const char* get_allocation_err_msg( ALLOCATION_ERRORS err ){
	if( err >= NO_ALLOCATION_ERRORS && err < ALLOCATION_ERR_ENDS ){
		const char *msg = allocation_error_msgs[err];
        const char *prefix = _ALLOC_ERR_TYPE;
       	return get_full_error_msg( prefix, msg );
	}
	return NULL;
}

ALLOCATION_ERRORS get_allocation_err( void ){
	ALLOCATION_ERRORS err = ALLOCATION_ERR_RAISED;
	ALLOCATION_ERR_RAISED = NO_ALLOCATION_ERRORS;
	return err;
}

size_t get_total_arena_allocated( void ){
	return total_arena_allocated;
}

static inline size_t align_up(size_t offset, size_t alignment) {
    return (offset + alignment - 1) & ~(alignment - 1);
}

bool alloc_next( void ){
	_memory* new_block = malloc( sizeof( _memory ) );
	if( !new_block ) {
		ALLOCATION_ERR_RAISED = ALLOCATION_MEMORY_FAILED; 
		return false;
	}
	new_block->mem = ( unsigned char* ) malloc( sizeof (unsigned char) * _EACH_BLOCK_SIZE );
	if( !new_block->mem ){
		free( new_block );
		ALLOCATION_ERR_RAISED = ALLOCATION_BLOCK_FAILED;
		return false;
	}
	new_block->curr_alloc = 0;
	new_block->next_stack = NULL;
	if ( !_mem_head  )
		_mem_head = new_block;
	else{
		new_block->next_stack = _mem_head;
		_mem_head = new_block;
	}
	total_arena_allocated++;
	return true;
}

void* _malloc(size_t alloc_size){
	if( !_mem_head ||  _EACH_BLOCK_SIZE - _mem_head->curr_alloc < alloc_size )
		if( !alloc_next() )
			return NULL;
	size_t offset = align_up( _mem_head->curr_alloc, 8 );
	if( alloc_size > _EACH_BLOCK_SIZE - offset ){
		if( !alloc_next() )
			return NULL;
		offset = align_up( _mem_head->curr_alloc, 8 );
	}
	void* new_allc_ptr = _mem_head->mem + offset;
	_mem_head->curr_alloc = offset + alloc_size;
	
	return new_allc_ptr;	
}

void free_memory( void ){
	while ( _mem_head ){
		_memory* next_node = _mem_head->next_stack;
		free ( _mem_head->mem );
		free ( _mem_head );
		_mem_head = next_node;
	}
}
#ifndef STACK_H
#define STACK_H

#include "alloc.h"
#include "error.h"

typedef enum {
	NO_STACK_ERRORS		,
	STACK_UNDERFLOW		,
	STACK_UNINITIALIZED ,
	STACK_ALLOCATION_FAILED,
	STACK_ERR_ENDS
}STACK_ERRORS;

static const char* stack_error_msgs[] = {
	"no stack errors\n", 
	"stack underflow\n",
	"stack uninitialized\n",
	"stack allocation failed\n",
};

extern const char*  _STACK_ERR_TYPE;
extern STACK_ERRORS STACK_ERR_RAISED;

const char*  get_stack_error_msg( STACK_ERRORS err_raised );
STACK_ERRORS get_stack_error 	 ( void );
#define STACK_LIMIT 15

typedef struct _stack{
	void* 			curr_node;
	struct _stack*  next_node;
}STACK;

STACK* 	get_stack_node	( void* );
bool 	push_stack 		( void*, STACK**  );
bool 	pop_stack		( STACK** stack );
void* 	top				( STACK** stack );
void 	clear_full_stack( STACK** stack );

#endif
#include "stack.h"
#include "error.h"


const char* _STACK_ERR_TYPE = "Stack error: ";
STACK_ERRORS STACK_ERR_RAISED = NO_STACK_ERRORS;

const char* get_stack_error_msg( STACK_ERRORS err_raised ){
	if ( err_raised >= NO_STACK_ERRORS && err_raised < STACK_ERR_ENDS ) {
        const char *msg = stack_error_msgs[ err_raised ];
        const char *prefix = _STACK_ERR_TYPE;
       	return get_full_error_msg( prefix, msg );
     }
    return NULL;
}

STACK_ERRORS get_stack_error( void ){
	STACK_ERRORS err_raised = STACK_ERR_RAISED;
	STACK_ERR_RAISED = NO_STACK_ERRORS;
	return err_raised;
}

STACK* get_stack_node( void* j_node ){
	STACK* new_stack = _malloc( sizeof( STACK ) );

	if( new_stack == NULL ){
		STACK_ERR_RAISED = STACK_ALLOCATION_FAILED;
		return NULL;
	}
	new_stack->curr_node = j_node;
	new_stack->next_node = NULL;
	return new_stack;
}

bool push_stack ( void* data, STACK** stack ){
	if( stack == NULL ){
		STACK_ERR_RAISED = STACK_UNINITIALIZED;
		return false;
	}
	STACK* n_stack_elem = get_stack_node( data );
	if( n_stack_elem  == NULL ) 
		return false;
	n_stack_elem->next_node = *stack;
	*stack = n_stack_elem;
	return true;
}

bool pop_stack( STACK** stack ){
	if( stack == NULL ){
		STACK_ERR_RAISED = STACK_UNINITIALIZED;
		return false;
	}
	if( *stack == NULL ) {
		STACK_ERR_RAISED = STACK_UNDERFLOW;
		return false;
	}
	*stack = (*stack)->next_node;
	return true;
}

void* top( STACK** stack ){
	if( stack == NULL ){
		STACK_ERR_RAISED = STACK_UNINITIALIZED;
		return NULL;
	}
	if( *stack == NULL ){
		STACK_ERR_RAISED = STACK_UNINITIALIZED;
		return NULL;
	}
	return (*stack)->curr_node;
}

void clear_full_stack( STACK **stack ){
	if( stack == NULL ){
		STACK_ERR_RAISED = STACK_UNINITIALIZED;
		return;
	}
	while( *stack ){
		STACK* next_node = (*stack)->next_node;
		*stack = next_node;
	}
}
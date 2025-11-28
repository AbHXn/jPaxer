#ifndef JPAXER_H
#define JPAXER_H

#include "parser.h"
#include "display.h"


typedef struct _json_traverser JSON_TR;
typedef void (*jFUNC)(JSON_TR**,const char*);

void   JSON_FLUSH(JSON_NODE* , FILE*, int);

void   __read__	 ( JSON_NODE**, const char* );
void   __update__( JSON_NODE**, const char*, void*, JSON_DTYPE );
void   __delete__( JSON_NODE**, const char* );
void   __create__( JSON_NODE**, const char*, void*, JSON_DTYPE );
void   __back__	 ( JSON_NODE** );

#define get(j_node,key) 			__read__((&j_node),(key))
#define set(j_node,key,value,dtype) __update__((&j_node),(key),(value),(dtype))
#define add(j_node,key,value,dtype) __create__((&j_node),(key),(value),(dtype))
#define del(j_node,key)				__delete__((&j_node),(key))
#define back(j_node)				__back__((&j_node))
#endif
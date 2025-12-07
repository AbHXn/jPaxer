#include "error.h"

const char* get_full_error_msg(  const char *msg,  const char *prefix ){
    if (!msg || !prefix) return NULL;
    
    size_t total_size = strlen(prefix) + strlen(msg) + 1;
    char *final_msg = _malloc(total_size);
    
    if (!final_msg) return NULL;
    
    strcpy(final_msg, prefix);
    strcat(final_msg, msg);
    return final_msg; 
}

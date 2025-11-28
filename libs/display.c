#include "display.h"

 void print_node(JSON_NODE* node, int level) {
    for (int i = 0; i < level; i++) printf("  "); // indentation
    printf(C_KEY "\"%s\"" C_RESET ": ", node->key);
    switch (node->dtype) {
        case J_STRING:
            printf(C_STRING "\"%s\"" C_RESET, node->value.string_val);
            break;
        case J_INT:
            printf(C_NUMBER "%ld" C_RESET, *(node->value.int_val));
            break;
        case J_DOUBLE:
            printf(C_NUMBER "%lf" C_RESET, *(node->value.double_val));
            break;
        case J_BOOL:
            printf(C_BOOL "%s" C_RESET,
                   *(node->value.bool_val) ? "true" : "false");
            break;
        case J_NULL:
            printf(C_NULL "null" C_RESET);
            break;
        case J_OBJECT:
            printf(C_BRACE "{\n" C_RESET);
            jobject* cur = node->value.object_val;
            while (cur) {
                print_node(cur->j_node, level + 1);
                if (cur->next) printf(",\n");
                else printf("\n");
                cur = cur->next;
            }
            for (int i = 0; i < level; i++) printf("  ");
            printf(C_BRACE "}" C_RESET);
            break;

        default:
            printf("(unknown)");
    }
}

void print_JSON_node(JSON_NODE* root) {
    if (!root) return;

    printf(C_BRACE "{\n" C_RESET);
    if( root->dtype != J_OBJECT ){
        print_node( root, 1 );
        printf("\n");
    }else{
        jobject* cur = root->value.object_val;
        while (cur) {
            print_node(cur->j_node, 1);
            if (cur->next) printf(",\n");
            else printf("\n");
            cur = cur->next;
        }
    }
    printf(C_BRACE "}\n" C_RESET);
}
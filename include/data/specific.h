#ifndef LAB_1_SPEC_H
#define LAB_1_SPEC_H

#include "common.h"

uint32_t* check_constraints_create_relationship(
    Cursor* cursor, Relationship* relationship,
    FunctionHelper* function_helper
);

uint32_t* check_constraints_create_property(
    Cursor* cursor, Property* property,
    FunctionHelper* function_helper
);

bool delete_node_with_others(Cursor* cursor, Node* node) ;

void write_big_string_to_file(Cursor* cursor, PageHeader* page_header, Entity* entity, uint32_t length, char* string);

void read_big_string_from_file(Cursor* cursor, PageHeader* page_header, char* body, char* string, uint32_t length, uint64_t* offset, uint32_t* read_block) ;

#endif

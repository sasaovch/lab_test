#ifndef LAB_1_RELATIONSHIP_H
#define LAB_1_RELATIONSHIP_H

#include "../io/io.h"

void print_relationship(Relationship* relationship);

void write_relationship_to_file(Cursor* cursor, PageHeader* page_header, Entity* entity, void* rel);

uint64_t get_size_of_relationship(void* rl);

uint32_t relationship_work_with_id(void* rl, uint32_t id, bool is_setter);

void* read_relationship(Cursor* cursor, PageHeader* page_header, void* element, const uint64_t* offset, char* body, uint32_t* read_block);

bool compare_relationship(void* rl_1, void* rl_2);

bool compare_parent_relationship(void* rl_1, void* rl_2);

bool compare_child_relationship(void* rl_1, void* rl_2);

bool compare_id_relationship(void* rl_1, void* rl_2);

bool compare_relationship_by_node(void* rl_1, void* rl_2);

void memcpy_relationship(void* element, char* stack, uint64_t* offset);

void* memget_relationship(void* element, char* stack, uint64_t* offset);

#endif

#ifndef LAB_1_PROPERTY_H
#define LAB_1_PROPERTY_H

#include "../io/io.h"

void write_property_to_file(Cursor* cursor, PageHeader* page_header, Entity* entity, void* pr);

void print_property(Property* property);

void* read_property(Cursor* cursor, PageHeader* page_header, void* element, const uint64_t* offset_, char* body, uint32_t* read_block);

uint64_t get_size_of_property(void* pr);

void memcpy_property(void* element, char* stack, uint64_t* offset);

void* memget_property(void* element, char* stack, uint64_t* offset);

uint32_t property_work_with_id(void* pr, uint32_t id, bool is_setter);

bool compare_property_by_id(void* pr_1, void* pr_2);

bool compare_subject_property(void* pr_1, void* pr_2) ;

bool compare_key_property(void* pr_1, void* pr_2) ;

#endif

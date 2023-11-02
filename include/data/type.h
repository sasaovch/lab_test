#ifndef LAB_1_TYPE_H
#define LAB_1_TYPE_H

#include "./data.h"
#include "./page_utils.h"

void print_type(Entity* entity);

void save_entity_and_create_new(Cursor* cursor);

void create_page_for_type(Cursor* cursor);

Entity* get_entity(Cursor* cursor, TypeOfElement element_type, const char* name, uint64_t* pointer, Entity* entity);

bool create_type(Cursor* cursor, Entity* entity);

bool delete_type(Cursor* cursor, Entity* entity);

EntityIterator* get_entity_iterator(Cursor* cursor, TypeOfElement element_type);

#endif

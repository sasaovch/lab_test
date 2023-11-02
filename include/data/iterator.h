#ifndef LAB_1_ITERATOR_H
#define LAB_1_ITERATOR_H

#include "data.h"
#include "../io/io.h"

void* next(Iterator* iterator);

bool has_next(Iterator* iterator);

void* entity_next(EntityIterator* entity_iterator);

bool entity_has_next(EntityIterator* entity_iterator);

void free_iter(Iterator* iterator);

void free_entity_iter(EntityIterator* entity_iterator);

#endif

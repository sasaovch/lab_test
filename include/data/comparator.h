#ifndef LAB_1_COMPARATOR_H
#define LAB_1_COMPARATOR_H

#include "data.h"

bool is_less(void* v1, void* v2, ValueType type);
bool is_greater(void* v1, void* v2, ValueType type);
bool is_equal(void* v1, void* v2, ValueType type);
bool return_true(void* d, void* n);

#endif

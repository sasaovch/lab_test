#include "../../include/data/comparator.h"
#include "data/data.h"

bool is_less(void* v1, void* v2, ValueType type) {
    switch (type) {
        case INT: {
            return *((uint32_t*) v1) < *((uint32_t*) v2);
        }
        case FLOAT: {
            return *((float*) v1) < *((float*) v2);
        }
        case BOOL: {
            return *((bool*) v1) < *((bool*) v2);
        }
        case STRING: {
            return strcmp(v1, v2) < 0;
        }
        case VOID: return false;
    }
}

bool is_greater(void* v1, void* v2, ValueType type) {
    switch (type) {
        case INT: {
            return *((uint32_t*) v1) > *((uint32_t*) v2);
        }
        case FLOAT: {
            return *((float*) v1) > *((float*) v2);
        }
        case BOOL: {
            return *((bool*) v1) > *((bool*) v2);
        }
        case STRING: {
            return strcmp(v1, v2) > 0;
        }
        case VOID: return false;
    }
}

bool is_equal(void* v1, void* v2, ValueType type) {
    switch (type) {
        case INT: {
            return *((uint32_t*) v1) == *((uint32_t*) v2);
        }
        case FLOAT: {
            return *((float*) v1) == *((float*) v2);
        }
        case BOOL: {
            return *((bool*) v1) == *((bool*) v2);
        }
        case STRING: {
            return strcmp(v1, v2) == 0;
        }
        case VOID: return false;
    }
}

bool return_true(void* d, void* n) {
    (void) d;
    (void) n;
    return true;
}

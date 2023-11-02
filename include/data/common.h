#ifndef LAB_1_COMMON_H
#define LAB_1_COMMON_H

#include "./type.h"
#include "data/data.h"

uint32_t* create_element(
    Cursor* cursor, void* element,
    TypeOfElement element_type, const char* type,
    FunctionHelper* function_helper,
    uint32_t* id
);

void* find_element(
    Cursor* cursor, Entity* entity, 
    Page* page, uint64_t size_of_element_malloc, 
    void* find_elem, uint64_t* offset_,
    FunctionHelper* function_helper
);

void* get_elements_by_condition(
    Cursor* cursor, Entity* entity, 
    uint64_t size_of_element_malloc, 
    void* helper, 
    FunctionHelper* function_helper
);

bool delete_element(
    Cursor* cursor, void* element, 
    uint64_t size_of_sturcture, 
    void* type, TypeOfElement element_type, 
    FunctionHelper* function_helper
);

bool update_element(
    Cursor* cursor, void* old_element, 
    void* new_element,
    uint64_t size_of_sturcture, 
    void *type, TypeOfElement element_type, 
    FunctionHelper* function_helper
);

void remove_bid_element(
    Cursor* cursor, PageHeader* page_header, 
    const uint64_t* pointer, uint64_t (*get_size_of_element)(void *), void* nd
);

Iterator* select_element(
    Cursor* cursor,
    TypeOfElement element_type, const char* type,
    uint64_t size_of_element_malloc, 
    void* helper, 
    FunctionHelper* function_helper
);

#endif

#ifndef LAB_1_UTIL_DATA_H
#define LAB_1_UTIL_DATA_H

#include "data.h"
#include <stdint.h>
#include <stdio.h>

#ifdef _WIN32
#include <io.h>
#define F_OK 0
#define access _access
#else
#include <unistd.h>
#endif

typedef struct {
    FILE* file;
    uint64_t file_length;
} File;

typedef struct {
    uint32_t block_number;
    uint32_t next_block;
    uint32_t offset;
} PageHeader;

typedef struct {
    PageHeader* page_header;
    char* body;
} Page;

typedef struct {
    File* file;
    uint32_t first_entity_block;
    uint32_t last_entity_block;
    uint32_t number_of_pages;
    Page* page;
} Cursor;

typedef enum {
    ID = 0,
    NAME = 1,
    SUBJECT = 2,
    PARENT = 3,
    CHILD = 4
} Field;

typedef enum {
    MORE = 0,
    LESS = 1,
    EQUALS = 2
} Operator;

typedef struct {
    TypeOfElement type_element;
    Field field;
    Operator Operator;
    ValueType type_value;
    uint32_t value;
    char* string_value;
} Request;

typedef struct {
    bool (*condition)(void*, void*);
    uint64_t (*get_size_of_element)(void*);
    void (*write_element_to_file)(Cursor*, PageHeader*, Entity*, void*);
    void* (*read_big_element)(Cursor*, PageHeader*, void* , const uint64_t* , char* , uint32_t*);
    void (*memcpy_element)(void* element, char* stack, uint64_t* offset);
    uint32_t (*work_with_id)(void* element, uint32_t id, bool is_setter);
} FunctionHelper;

typedef struct {
    uint64_t malloc_element_size;
    Cursor* cursor;
    PageHeader* page_header;
    Entity* entity;
    char* body;

    uint32_t* read_block_;
    
    uint64_t* offset_;
    void* element;
    void* helper;
    void* next_element;
    FunctionHelper* function_helper;
} Iterator;

typedef struct {
    Cursor* cursor;
    Entity* entity;
    PageHeader* page_header;
    char* body;

    uint32_t* read_block_;
    uint64_t* offset_;

    Iterator* iterator;
} EntityIterator;

#endif

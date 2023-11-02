#ifndef LAB_1_DATA_H
#define LAB_1_DATA_H

#include "constants.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

typedef enum {
  NODE = 0,
  RELATIONSHIP = 1,
  PROPERTY = 2,
} TypeOfElement;

typedef enum {
  INT = 0,
  FLOAT = 1,
  STRING = 2,
  BOOL = 3,
  VOID = 4,
} ValueType;

typedef struct {
    TypeOfElement element_type;
    ValueType value_type;
    uint32_t first_block;
    uint32_t last_block;
    uint32_t next_id;
    char type[NAME_TYPE_LENGTH + 1];
} Entity;

typedef struct {
    uint32_t id;
    uint32_t name_length;
    char type[NAME_TYPE_LENGTH + 1];
    char* name;
} Node;

typedef struct {
    uint32_t id;
    uint32_t parent_id;
    uint32_t child_id;
    char type[NAME_TYPE_LENGTH + 1];
    char parent_type[NAME_TYPE_LENGTH + 1];
    char child_type[NAME_TYPE_LENGTH + 1];
} Relationship;

typedef struct {
    ValueType value_type;
    uint32_t subject_id;
    uint32_t value_length;
    char type[NAME_TYPE_LENGTH + 1];
    char subject_type[NAME_TYPE_LENGTH + 1];
    void* value;
} Property;

#endif

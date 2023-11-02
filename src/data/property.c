#include "../../include/include.h"
#include "data/constants.h"
#include "io/io.h"
#include <stdint.h>
#include <stdlib.h>


void print_property(Property* property) {
    println("Print property");
    println("Value type - %i", property->value_type);
    println("Subject id - %llu", property->subject_id);
    println("Value length - %i", property->value_length);
    println("Type - %s", property->type);
    println("Subject type - %s", property->subject_type);

    switch (property->value_type) {
        case INT: 
            println("Value - %i", *((uint32_t*) property->value));
            break;
        case BOOL: 
            println("Value - %i", property->value);
            break;
        case FLOAT: 
            println("Value - %f", *((float*)property->value));
            break;
        case STRING: 
            println("Value - %s", property->value);
            break;
        case VOID: 
            println("Value - %s", property->value);
            break;
    }
}

void write_property_to_file(Cursor* cursor, PageHeader* page_header, Entity* entity, void* pr) {
    Property* property = (Property*) pr;

    write_uint_32_to_file(cursor, property->value_type);
    write_uint_32_to_file(cursor, property->subject_id);
    write_uint_32_to_file(cursor, property->value_length);
    write_string_to_file(cursor, property->type, NAME_TYPE_WITH_TERM_LENGTH);
    write_string_to_file(cursor, property->subject_type, NAME_TYPE_WITH_TERM_LENGTH);
    
    page_header->offset += VALUE_TYPE_SIZE + UINT32_T_SIZE * 2 + NAME_TYPE_SIZE * 2;

    switch (property->value_type) {
        case INT: {
            write_uint_32_to_file(cursor, *((uint32_t*)(property->value)));
            page_header->offset += UINT32_T_SIZE;
            break;
        }
        case BOOL: {
            write_to_file(cursor->file, property->value, sizeof(bool));
            page_header->offset += sizeof(bool);
            break;
        }
        case FLOAT: {
            write_to_file(cursor->file, property->value, sizeof(float));
            page_header->offset += sizeof(float);
            break;
        }
        case STRING: 
            write_big_string_to_file(cursor, page_header, entity, property->value_length, property->value);
            break;
        case VOID: 
            break;
    }
}

void* read_property(Cursor* cursor, PageHeader* page_header, void* element, const uint64_t* offset_, char* body, uint32_t* read_block) {
        Property* property = (Property*) element;

        uint32_t subject_id = 0;
        uint32_t value_length = 0;

        uint64_t offset = *offset_;
        
        memcpy(&(property->value_type), body + offset, VALUE_TYPE_SIZE);
        offset += VALUE_TYPE_SIZE;
        
        memcpy(&(subject_id), body + offset, UINT32_T_SIZE);
        offset += UINT32_T_SIZE;
        memcpy(&(value_length), body + offset, UINT32_T_SIZE);
        offset += UINT32_T_SIZE;
        
        memcpy(property->type, body + offset, NAME_TYPE_SIZE);
        offset += NAME_TYPE_SIZE;
        memcpy(property->subject_type, body + offset, NAME_TYPE_SIZE);
        offset += NAME_TYPE_SIZE;
        property->value_length = 0;
        
        switch (property->value_type) {
            case INT: {
                uint32_t* value = malloc(UINT32_T_SIZE);
                memcpy(value, body + offset, UINT32_T_SIZE);
                property->value = value;
                break;
            }
            case BOOL: {
                bool* value = malloc(sizeof(bool));
                memcpy(value, body + offset, sizeof(bool));
                property->value = value;
            }
            case FLOAT: {
                float* value = malloc(sizeof(float));
                memcpy(value, body + offset, sizeof(float));
                property->value = value;
                break;
            }
            case STRING: {
                char* value = malloc(CHAR_SIZE * value_length);
                read_big_string_from_file(cursor, page_header, body, value, value_length, &(offset), read_block);
                property->value = value;
                property->value_length = value_length;
                break;
            }
            case VOID: 
                break;
        }
                    
        property->subject_id = subject_id;

        return property;
}

uint64_t get_size_of_property(void* pr) {
    Property* property = (Property*) pr;
    uint64_t size = VALUE_TYPE_SIZE + UINT32_T_SIZE * 2 + NAME_TYPE_SIZE * 2;

    switch (property->value_type) {
        case INT: 
            size += UINT32_T_SIZE;
            break;
        case BOOL: 
            size += sizeof(bool);
            break;
        case FLOAT: 
            size += sizeof(float);
            break;
        case STRING: 
            size += CHAR_SIZE * property->value_length;
            break;
        case VOID: 
            break;
    }

    return size;
}

bool compare_property_by_id(void* pr_1, void* pr_2) {
    Property* property_1 = (Property*) pr_1;
    Property* property_2 = (Property*) pr_2;
    return property_1->subject_id == property_2->subject_id;
}

bool compare_subject_property(void* pr_1, void* pr_2) {
    Property* property_1 = (Property*) pr_1;
    Property* property_2 = (Property*) pr_2;
    return property_1->subject_id == property_2->subject_id &&
        strcmp(property_1->subject_type, property_2->subject_type) == 0;
}

bool compare_key_property(void* pr_1, void* pr_2) {
    Property* property_1 = (Property*) pr_1;
    Property* property_2 = (Property*) pr_2;
    return property_1->subject_id == property_2->subject_id;
}

bool is_value_greater(void* pr_1, void* pr_2) {
    Property* property_1 = (Property*) pr_1;
    Property* property_2 = (Property*) pr_2;
    return is_greater(property_1->value, property_2->value, property_1->value_type);
}
bool is_value_less(void* pr_1, void* pr_2) {
    Property* property_1 = (Property*) pr_1;
    Property* property_2 = (Property*) pr_2;
    return is_less(property_1->value, property_2->value, property_1->value_type);
}
bool is_value_equal(void* pr_1, void* pr_2) {
    Property* property_1 = (Property*) pr_1;
    Property* property_2 = (Property*) pr_2;
    return is_equal(property_1->value, property_2->value, property_1->value_type);
}

void memcpy_property(void* element, char* stack, uint64_t* offset) {
    Property* property = (Property*) element;

    uint32_t value_length = property->value_length;

    memcpy(stack + *offset, &(property->value_type), VALUE_TYPE_SIZE);
    *offset += VALUE_TYPE_SIZE;

    memcpy(stack + *offset, &(property->subject_id), UINT32_T_SIZE);
    *offset += UINT32_T_SIZE;

    memcpy(stack + *offset, &(value_length), UINT32_T_SIZE);
    *offset += UINT32_T_SIZE;

    memcpy(stack + *offset, property->type, NAME_TYPE_SIZE);
    *offset += NAME_TYPE_SIZE;
    
    memcpy(stack + *offset, property->subject_type, NAME_TYPE_SIZE);
    *offset += NAME_TYPE_SIZE;

    memcpy(stack + *offset, property->value, CHAR_SIZE * value_length);
    *offset += CHAR_SIZE * value_length;
}

void* memget_property(void* element, char* stack, uint64_t* offset) {
    Property* property = (Property*) element;

    uint32_t subject_id = 0;
    uint32_t value_length = 0;

    memcpy(&(property->value_type), stack + *offset, VALUE_TYPE_SIZE);
    *offset += VALUE_TYPE_SIZE;

    memcpy(&(subject_id), stack + *offset, UINT32_T_SIZE);
    *offset += UINT32_T_SIZE;

    memcpy(&(value_length), stack + *offset, UINT32_T_SIZE);
    *offset += UINT32_T_SIZE;

    memcpy(property->type, stack + *offset, NAME_TYPE_SIZE);
    *offset += NAME_TYPE_SIZE;

    memcpy(property->subject_type, stack + *offset, NAME_TYPE_SIZE);
    *offset += NAME_TYPE_SIZE;

    char* value = malloc(CHAR_SIZE * value_length);
    memcpy(value, stack + *offset, CHAR_SIZE * value_length);
    *offset += CHAR_SIZE * value_length;
    
    property->subject_id = subject_id;
    property->value_length = value_length;
    property->value = value;

    return property;
}

uint32_t property_work_with_id(void* pr, uint32_t id, bool is_setter) {
    (void) pr;
    (void) id;
    (void) is_setter;
    return 0;
}

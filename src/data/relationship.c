#include "../../include/include.h"


void print_relationship(Relationship* relationship) {
    println("Print relationship");
    println("Id - %llu", relationship->id);
    println("Parent - %llu", relationship->parent_id);
    println("Child - %llu", relationship->child_id);
    println("Type - %s", relationship->type);
    println("Parent Type - %s", relationship->parent_type);
    println("Child Type - %s", relationship->child_type);
}

uint64_t get_size_of_relationship(void* rl) {
    (void) rl;
    return RELATIONSHIP_SIZE;
}

void write_relationship_to_file(Cursor* cursor, PageHeader* page_header, Entity* entity, void* rel) {
    (void) page_header;
    (void) entity;
    Relationship* relationship = (Relationship*) rel;

    write_uint_32_to_file(cursor, relationship->id);
    write_uint_32_to_file(cursor, relationship->parent_id);
    write_uint_32_to_file(cursor, relationship->child_id);
    write_string_to_file(cursor, relationship->type, NAME_TYPE_WITH_TERM_LENGTH);
    write_string_to_file(cursor, relationship->parent_type, NAME_TYPE_WITH_TERM_LENGTH);
    write_string_to_file(cursor, relationship->child_type, NAME_TYPE_WITH_TERM_LENGTH);
    
    page_header->offset += RELATIONSHIP_SIZE;
}

void* read_relationship(Cursor* cursor, PageHeader* page_header, void* element, const uint64_t* offset_, char* body, uint32_t* read_block) {
        (void) cursor;
        (void) page_header;
        (void) read_block;

        Relationship* relationship = (Relationship*) element;

        uint32_t id = 0;
        uint32_t parent_id = 0;
        uint32_t child_id = 0;

        uint64_t offset = *offset_;
        
        memcpy(&(id), body + offset, UINT32_T_SIZE);
        offset += UINT32_T_SIZE;
        memcpy(&(parent_id), body + offset, UINT32_T_SIZE);
        offset += UINT32_T_SIZE;
        memcpy(&(child_id), body + offset, UINT32_T_SIZE);
        offset += UINT32_T_SIZE;
        
        memcpy(relationship->type, body + offset, NAME_TYPE_SIZE);
        offset += NAME_TYPE_SIZE;
        memcpy(relationship->parent_type, body + offset, NAME_TYPE_SIZE);
        offset += NAME_TYPE_SIZE;
        memcpy(relationship->child_type, body + offset, NAME_TYPE_SIZE);
        offset += NAME_TYPE_SIZE;
        
        relationship->id = id;
        relationship->parent_id = parent_id;
        relationship->child_id = child_id;

        return relationship;
}

bool compare_relationship(void* rl_1, void* rl_2) {
    Relationship* relationship_1 = (Relationship*) rl_1;
    Relationship* relationship_2 = (Relationship*) rl_2;
    return relationship_1->parent_id == relationship_2->parent_id && 
        relationship_1->id == relationship_2->id && relationship_1->child_id == relationship_2->id
        && strcmp(relationship_1->type, relationship_2->type) == 0;
}

bool compare_parent_relationship(void* rl_1, void* rl_2) {
    Relationship* relationship_1 = (Relationship*) rl_1;
    Relationship* relationship_2 = (Relationship*) rl_2;
    return relationship_1->parent_id == relationship_2->parent_id &&
        strcmp(relationship_1->parent_type, relationship_2->parent_type) == 0;
}

bool compare_relationship_by_node(void* rl_1, void* rl_2) {
    Relationship* relationship_1 = (Relationship*) rl_1;
    Relationship* relationship_2 = (Relationship*) rl_2;
    return (strcmp(relationship_1->parent_type, relationship_2->parent_type) == 0 &&
        relationship_1->parent_id == relationship_2->parent_id) ||
        (strcmp(relationship_1->child_type, relationship_2->child_type) == 0 &&
        relationship_1->child_id == relationship_2->child_id);
}

bool compare_child_relationship(void* rl_1, void* rl_2) {
    Relationship* relationship_1 = (Relationship*) rl_1;
    Relationship* relationship_2 = (Relationship*) rl_2;
    return relationship_1->child_id == relationship_2->child_id &&
        strcmp(relationship_1->child_type, relationship_2->child_type) == 0;
}

bool compare_id_relationship(void* rl_1, void* rl_2) {
    Relationship* relationship_1 = (Relationship*) rl_1;
    Relationship* relationship_2 = (Relationship*) rl_2;
    return relationship_1->id == relationship_2->id &&
        strcmp(relationship_1->type, relationship_2->type) == 0;
}

void memcpy_relationship(void* element,char* stack, uint64_t* offset) {
    Relationship* relationship = (Relationship*) element;

    memcpy(stack + *offset, &(relationship->id), UINT32_T_SIZE);
    *offset += UINT32_T_SIZE;

    memcpy(stack + *offset, &(relationship->parent_id), UINT32_T_SIZE);
    *offset += UINT32_T_SIZE;
    
    memcpy(stack + *offset, &(relationship->child_id), UINT32_T_SIZE);
    *offset += UINT32_T_SIZE;
    
    memcpy(stack + *offset, relationship->type, NAME_TYPE_SIZE);
    *offset += NAME_TYPE_SIZE;
    
    memcpy(stack + *offset, relationship->parent_type, NAME_TYPE_SIZE);
    *offset += NAME_TYPE_SIZE;
    
    memcpy(stack + *offset, relationship->child_type, NAME_TYPE_SIZE);
    *offset += NAME_TYPE_SIZE;
}

void* memget_relationship(void* element, char* stack, uint64_t* offset) {
    Relationship* relationship = (Relationship*) element;

    uint32_t id = 0;
    uint32_t parent_id = 0;
    uint32_t child_id = 0;

    memcpy(&(id), stack + *offset, UINT32_T_SIZE);
    *offset += UINT32_T_SIZE;

    memcpy(&(parent_id), stack + *offset, UINT32_T_SIZE);
    *offset += UINT32_T_SIZE;

    memcpy(&(child_id), stack + *offset, UINT32_T_SIZE);
    *offset += UINT32_T_SIZE;

    memcpy(relationship->type, stack + *offset, NAME_TYPE_SIZE);
    *offset += NAME_TYPE_SIZE;
    
    memcpy(relationship->parent_type, stack + *offset, NAME_TYPE_SIZE);
    *offset += NAME_TYPE_SIZE;
    
    memcpy(relationship->child_type, stack + *offset, NAME_TYPE_SIZE);
    *offset += NAME_TYPE_SIZE;

    relationship->id = id;
    relationship->parent_id = parent_id;
    relationship->child_id = child_id;

    return relationship;
}

uint32_t relationship_work_with_id(void* rl, uint32_t id, bool is_setter) {
    Relationship* relationship = (Relationship*) rl;
    if (is_setter) {
        relationship->id = id;
        return id;
    }
    return relationship->id;
}

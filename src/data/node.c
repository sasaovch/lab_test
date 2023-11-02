#include "../../include/include.h"


void print_node(Node* node) {
    println("Print node");
    println("Id - %llu", node->id);
    println("Type - %s", node->type);
    println("Length - %i", node->name_length);
    println("Name - %s", node->name);
}

void write_node_to_file(Cursor* cursor, PageHeader* page_header, Entity* entity, void* nd) {
    Node* node = (Node*) nd;

    write_uint_32_to_file(cursor, node->id);
    write_uint_32_to_file(cursor, node->name_length);
    write_string_to_file(cursor, node->type, NAME_TYPE_WITH_TERM_LENGTH);

    page_header->offset += UINT32_T_SIZE * 2 + NAME_TYPE_SIZE;

    write_big_string_to_file(cursor, page_header, entity, node->name_length, node->name);
}

void* read_node(Cursor* cursor, PageHeader* page_header, void* element, const uint64_t* offset_, char* body, uint32_t* read_block) {
        Node* node = (Node*) element;

        uint32_t id = 0;
        uint32_t name_length = 0;

        uint64_t offset = *offset_;
        
        memcpy(&(id), body + offset, UINT32_T_SIZE);
        offset += UINT32_T_SIZE;
        memcpy(&(name_length), body + offset, UINT32_T_SIZE);
        offset += UINT32_T_SIZE;
        memcpy(node->type, body + offset, NAME_TYPE_SIZE);
        offset += NAME_TYPE_SIZE;
        
        char* name = malloc(CHAR_SIZE * name_length);
        read_big_string_from_file(cursor, page_header, body, name, name_length, &(offset), read_block);
            
        node->id = id;
        node->name_length = name_length;
        node->name = name;

        return node;
}

uint64_t get_size_of_node(void* nd) {
    Node* node = (Node*) nd;
    return UINT32_T_SIZE + UINT32_T_SIZE + CHAR_SIZE * node->name_length + NAME_TYPE_SIZE;
}

bool compare_nodes(void* nd_1, void* nd_2) {
    Node* node_1 = (Node*) nd_1;
    Node* node_2 = (Node*) nd_2;
    return (strcmp(node_1->name, node_2->name) == 0) && (node_1->type == node_2->type) == 0;
}

bool compare_id_node(void* nd_1, void* nd_2) {
    Node* node_1 = (Node*) nd_1;
    Node* node_2 = (Node*) nd_2;
    return node_1->id == node_2->id &&
        strcmp(node_1->type, node_2->type) == 0;
}

bool greater_id_node(void* nd_1, void* nd_2) {
    Node* node_1 = (Node*) nd_1;
    Node* node_2 = (Node*) nd_2;
    return node_1->id < node_2->id;
}

bool compare_name_node(void* nd_1, void* nd_2) {
    Node* node_1 = (Node*) nd_1;
    Node* node_2 = (Node*) nd_2;
    return (strcmp(node_1->name, node_2->name) == 0);
}

void memcpy_node(void* element, char* stack, uint64_t* offset) {
    Node* node = (Node*) element;

    memcpy(stack + *offset, &(node->id), UINT32_T_SIZE);
    *offset += UINT32_T_SIZE;

    memcpy(stack + *offset, &(node->name_length), UINT32_T_SIZE);
    *offset += UINT32_T_SIZE;
    
    memcpy(stack + *offset, node->type, NAME_TYPE_SIZE);
    *offset += NAME_TYPE_SIZE;

    memcpy(stack + *offset, node->name, CHAR_SIZE * node->name_length);
    *offset += CHAR_SIZE * node->name_length;
}

void* memget_node(void* element, char* stack, uint64_t* offset) {
    Node* node = (Node*) element;
    uint32_t name_length = 0;
    uint32_t id = 0;

    memcpy(&(id), stack + *offset, UINT32_T_SIZE);
    *offset += UINT32_T_SIZE;

    memcpy(&(name_length), stack + *offset, UINT32_T_SIZE);
    *offset += UINT32_T_SIZE;

    memcpy(node->type, stack + *offset, NAME_TYPE_SIZE);
    *offset += NAME_TYPE_SIZE;

    void* name = malloc(CHAR_SIZE * name_length);
    memcpy(name, stack + *offset, CHAR_SIZE * name_length);
    *offset += CHAR_SIZE * name_length;

    node->id = id;
    node->name_length = name_length;
    node->name = name;

    return node;
}


uint32_t node_work_with_id(void* nd, uint32_t id, bool is_setter) {
    Node* node = (Node*) nd;
    if (is_setter) {
        node->id = id;
        return id;
    }
    return node->id;
}

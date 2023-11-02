#include "../../include/include.h"
#include "data/constants.h"
#include "data/iterator.h"
#include <string.h>

#define _CRT_SECURE_NO_WARNINGS
uint32_t* check_constraints_create_relationship(
    Cursor* cursor, Relationship* relationship,
    FunctionHelper* function_helper
) {
    Node* nd = (Node*) malloc(NODE_SIZE);
    nd->id = relationship->parent_id;
    //strlcpy(nd->type, relationship->parent_type, (size_t)NAME_TYPE_WITH_TERM_LENGTH);
    
    Iterator* iter = select_node_by_id(cursor, nd);
    if (!has_next(iter)) {
        return NULL;
    }

    nd->id = relationship->child_id;
    //strlcpy(nd->type, relationship->child_type, (size_t)NAME_TYPE_WITH_TERM_LENGTH);

    iter = select_node_by_id(cursor, nd);
    if (!has_next(iter)) {
        return NULL;
    }

    free(iter);
    free(nd);

    return create_element(cursor, relationship, RELATIONSHIP, relationship->type, function_helper, NULL);
}

uint32_t* check_constraints_create_property(
    Cursor* cursor, Property* property,
    FunctionHelper* function_helper
) {
    Node* nd = (Node*) malloc(NODE_SIZE);
    nd->id = property->subject_id;
    //strlcpy(nd->type, property->subject_type, (size_t)NAME_TYPE_WITH_TERM_LENGTH);
    
    Iterator* iter = select_node_by_id(cursor, nd);
    if (!has_next(iter)) {
        return NULL;
    }

    iter = select_property_by_subject(cursor, property);
    if (has_next(iter)) {
        return NULL;
    }

    Entity* entity = (Entity*) malloc(ENTITY_SIZE);
    PageHeader* page_header = (PageHeader*) malloc(PAGE_HEADER_SIZE);
    uint64_t pointer = 0;
    
    entity = get_entity(cursor, PROPERTY, property->type, &(pointer), entity);
    if (entity == NULL) {
        free(entity);
        free(page_header);
        println("Error to find Entity with name %s", property->type);
        return NULL;
    }
    if (entity->value_type != property->value_type) {
        return NULL;
    }
    free(page_header);
    free(entity);
    free(iter);
    free(nd);

    return create_element(cursor, property, PROPERTY, property->type, function_helper, NULL);
}

void delete_relationship_iter(EntityIterator* entity_iterator) {
    Iterator* iterator = entity_iterator->iterator;
    uint64_t offset = *(iterator->offset_) - RELATIONSHIP_SIZE;
    uint32_t read_block = *(iterator->read_block_);
    
    Cursor* cursor = iterator->cursor;
    PageHeader* page_header = iterator->page_header;
    Entity* entity = iterator->entity;
    
    char* body = iterator->body;
    uint64_t page_offset = read_block * PAGE_SIZE;

    char* new_body = malloc(PAGE_BODY_SIZE);
    
    set_pointer_offset_file(cursor->file, page_offset);
    read_from_file(cursor->file, page_header, PAGE_HEADER_SIZE);  
    read_from_file(cursor->file, body, PAGE_BODY_SIZE);

    memcpy(new_body, body, offset);
    memcpy(new_body + offset, body + offset + RELATIONSHIP_SIZE, PAGE_BODY_SIZE - offset - RELATIONSHIP_SIZE);

    uint64_t new_offset = page_header->offset - RELATIONSHIP_SIZE;
        
    set_pointer_offset_file(cursor->file, page_header->block_number * PAGE_SIZE + PAGE_HEADER_SIZE - UINT32_T_SIZE);
    write_to_file(cursor->file, &(new_offset), UINT32_T_SIZE);
    write_to_file(cursor->file, new_body, PAGE_BODY_SIZE);
        
    uint32_t new_block_number = page_header->block_number;
    if (new_offset == 0) {
        uint64_t pointer = *(entity_iterator->offset_) - ENTITY_SIZE + PAGE_HEADER_SIZE + *(entity_iterator->read_block_) * PAGE_SIZE;
        cut_blocks(cursor, page_header, &(pointer), entity);
        new_block_number = find_block_before(cursor, page_header, entity);
    }
    *(entity_iterator->iterator->offset_) = new_offset;
    *(entity_iterator->iterator->read_block_) = new_block_number;

    free(new_body);
}

void delete_property_iter(EntityIterator* entity_iterator) {
    Property* delete_prop = entity_iterator->iterator->next_element;
    Iterator* iterator = entity_iterator->iterator;
    
    Cursor* cursor = iterator->cursor;
    PageHeader* page_header = iterator->page_header;
    Entity* entity = iterator->entity;

    uint64_t size_of_element = iterator->function_helper->get_size_of_element(delete_prop);
    uint64_t offset = *(iterator->offset_) - size_of_element;
    
    uint32_t read_block = *(iterator->read_block_);
    uint64_t page_offset = read_block * PAGE_SIZE;
    
    char* body = iterator->body;
    char* new_body = malloc(PAGE_BODY_SIZE);

    set_pointer_offset_file(cursor->file, page_offset);
    read_from_file(cursor->file, page_header, PAGE_HEADER_SIZE);  
    read_from_file(cursor->file, body, PAGE_BODY_SIZE);
        
    if (size_of_element <= PAGE_BODY_SIZE) {
        memcpy(new_body, body, offset);
        memcpy(new_body + offset, body + offset + size_of_element, PAGE_BODY_SIZE - offset - size_of_element);

        uint64_t new_offset = page_header->offset - size_of_element;
            
        set_pointer_offset_file(cursor->file, page_header->block_number * PAGE_SIZE + PAGE_HEADER_SIZE - UINT32_T_SIZE);
        write_to_file(cursor->file, &(new_offset), UINT32_T_SIZE);
        write_to_file(cursor->file, new_body, PAGE_BODY_SIZE);
            
        uint32_t new_block_number = page_header->block_number;
        if (new_offset == 0) {
            uint64_t pointer = *(entity_iterator->offset_) - ENTITY_SIZE + PAGE_HEADER_SIZE + *(entity_iterator->read_block_) * PAGE_SIZE;
            cut_blocks(cursor, page_header, &(pointer), entity);
            new_block_number = find_block_before(cursor, page_header, entity);
        }
        *(entity_iterator->iterator->offset_) = new_offset;
        *(entity_iterator->iterator->read_block_) = new_block_number;
    } else {
        uint64_t pointer = *(entity_iterator->offset_) - ENTITY_SIZE + PAGE_HEADER_SIZE + *(entity_iterator->read_block_) * PAGE_SIZE;
        remove_bid_element(cursor, page_header, &(pointer), iterator->function_helper->get_size_of_element, delete_prop);
    }

    free(new_body);
}

bool delete_node_with_others(
        Cursor* cursor, Node* node
    ) {

    EntityIterator* entity_iterator_rel = select_relationships_by_node(cursor, node);
    while (entity_iterator_rel != NULL && entity_has_next(entity_iterator_rel)) {
        delete_relationship_iter(entity_iterator_rel);
    }

    EntityIterator* entity_iterator_prop = select_properties_by_node(cursor, node);
    while (entity_iterator_prop != NULL && entity_has_next(entity_iterator_prop)) {
        delete_property_iter(entity_iterator_prop);
    }
    free_entity_iter(entity_iterator_rel);
    free_entity_iter(entity_iterator_prop);
    
    FunctionHelper* function_helper = (FunctionHelper*) malloc(sizeof(FunctionHelper));
    function_helper->condition = compare_id_node;
    function_helper->get_size_of_element = get_size_of_node;
    function_helper->write_element_to_file = write_node_to_file;
    function_helper->read_big_element = read_node;
    
    bool result = delete_element(cursor, node, NODE_SIZE, node->type, NODE, function_helper);
    free(function_helper);
    return result;
}

void write_big_string_to_file(Cursor* cursor, PageHeader* page_header, Entity* entity, uint32_t length, char* string) {
    set_pointer_offset_file(cursor->file, page_header->block_number * PAGE_SIZE + page_header->offset + PAGE_HEADER_SIZE);
    uint32_t string_offset = 0;
    char* string_to_write = (char*) calloc(1, PAGE_BODY_SIZE);
    uint32_t page_offset = page_header->block_number * PAGE_SIZE;

    while (length != 0) {
        uint32_t write_length = 0;
        if (length <= PAGE_BODY_SIZE - page_header->offset) {
            write_length = length;
        } else {
            write_length = PAGE_BODY_SIZE - page_header->offset;
        }
        memcpy(string_to_write, string + string_offset, write_length);

        write_string_to_file(cursor, string_to_write, write_length);

        length -= write_length;
        string_offset += write_length;
        page_header->offset += write_length;

        if (page_header->offset == PAGE_BODY_SIZE && length != 0) {
            uint64_t header_offset = page_header->block_number * PAGE_SIZE + UINT32_T_SIZE * 2;
            set_pointer_offset_file(cursor->file, header_offset);
            write_to_file(cursor->file, &(page_header->offset), UINT32_T_SIZE);

            create_new_page(cursor, page_header, page_header->block_number * PAGE_SIZE);
        
            entity->last_block = cursor->number_of_pages;
            page_offset = entity->last_block * PAGE_SIZE;
            uint32_t global_offset = page_offset + PAGE_HEADER_SIZE;

            set_pointer_offset_file(cursor->file, global_offset);
        }
    }
    uint64_t header_offset = page_header->block_number * PAGE_SIZE + UINT32_T_SIZE * 2;
    set_pointer_offset_file(cursor->file, header_offset);
    write_to_file(cursor->file, &(page_header->offset), UINT32_T_SIZE);
    free(string_to_write);
}

void read_big_string_from_file(Cursor* cursor, PageHeader* page_header, char* body, char* string, uint32_t length, uint64_t* offset, uint32_t* read_block) {
    uint32_t to_read_length = 0;
    uint32_t string_offset = 0;
    
    while (length != 0) {

        if (length <= PAGE_BODY_SIZE - *offset) {
            to_read_length = length;
        } else {
            to_read_length = PAGE_BODY_SIZE - *offset;
        }
    
        memcpy(string + string_offset, body + *offset, to_read_length);
        *offset += to_read_length;
        string_offset += to_read_length;
        length -= to_read_length;

        if (*offset == PAGE_BODY_SIZE) {
            *read_block = page_header->next_block;
            uint64_t page_offset = *read_block * PAGE_SIZE;

            set_pointer_offset_file(cursor->file, page_offset);
            read_from_file(cursor->file, page_header, PAGE_HEADER_SIZE);  
            read_from_file(cursor->file, body, PAGE_BODY_SIZE);
            *offset = 0;
        }
    }
}

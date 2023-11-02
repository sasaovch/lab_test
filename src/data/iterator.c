#include "../../include/data/iterator.h"
#include "data/relationship.h"
#include "data/util_data.h"
#include <stdint.h>
#include <stdlib.h>


void* next(Iterator* iterator) {
        return iterator->next_element;
}

bool has_next(Iterator* iterator) {
    if (iterator == NULL) {
        return false;
    }
    uint64_t offset = *(iterator->offset_);
    uint32_t read_block = *(iterator->read_block_);

    Cursor* cursor = iterator->cursor;
    PageHeader* page_header = iterator->page_header;
    
    char* body = iterator->body;
    void* element = iterator->element;

    while (read_block != 0) {
        uint64_t page_offset = read_block * PAGE_SIZE;

        set_pointer_offset_file(cursor->file, page_offset);
        read_from_file(cursor->file, page_header, PAGE_HEADER_SIZE);  
        read_from_file(cursor->file, body, PAGE_BODY_SIZE);

        while (offset < page_header->offset) {

            element = iterator->function_helper->read_big_element(cursor, page_header, element, &(offset), body, &(read_block));

            if (iterator->function_helper->condition(element, iterator->helper)) {
                *(iterator->offset_) = offset + iterator->function_helper->get_size_of_element(element) % PAGE_BODY_SIZE;
                *(iterator->read_block_) = read_block;
                iterator->next_element = element;
                return true;
            }
            offset = (offset + iterator->function_helper->get_size_of_element(element)) % PAGE_BODY_SIZE;
        }
        read_block = page_header->next_block;
        offset = 0;
    }
    return false;
}

void* entity_next(EntityIterator* entity_iterator) {
        return entity_iterator->iterator->next_element;
}

bool entity_has_next(EntityIterator* entity_iterator) {
    Iterator* iterator = entity_iterator->iterator;
    uint64_t offset = *(iterator->offset_);
    uint32_t read_block = *(iterator->read_block_);
    
    Cursor* cursor = entity_iterator->cursor;
    PageHeader* page_header = iterator->page_header;
    Entity* entity = iterator->entity;
    
    char* body = iterator->body;
    void* element = iterator->element;

    while (read_block != 0) {
        uint64_t page_offset = read_block * PAGE_SIZE;

        set_pointer_offset_file(cursor->file, page_offset);
        read_from_file(cursor->file, page_header, PAGE_HEADER_SIZE);  
        read_from_file(cursor->file, body, PAGE_BODY_SIZE);

        while (offset < page_header->offset) {

            element = iterator->function_helper->read_big_element(cursor, page_header, element, &(offset), body, &(read_block));

            if (iterator->function_helper->condition(element, iterator->helper)) {
                iterator->cursor = cursor;
                iterator->page_header = page_header;
                iterator->entity = entity;
                uint64_t size_of_element = iterator->function_helper->get_size_of_element(element);
                *(iterator->offset_) = (offset + size_of_element) % PAGE_BODY_SIZE;
                *(iterator->read_block_) = read_block;
                iterator->next_element = element;
                return true;
            }
            uint64_t size_of_element = iterator->function_helper->get_size_of_element(element);
            offset = (offset + size_of_element) % PAGE_BODY_SIZE;
        }
        read_block = page_header->next_block;
        offset = 0;
    }

    char* entity_body = entity_iterator->body;
    PageHeader* entity_page_header = entity_iterator->page_header;
    Entity* new_entity = (Entity*) malloc(ENTITY_SIZE);
    uint32_t page_num = *(entity_iterator->read_block_);

    do {
        for (uint64_t i = *(entity_iterator->offset_) / ENTITY_SIZE; i < (entity_page_header->offset / ENTITY_SIZE); i++) {
            memcpy(new_entity, entity_body + i * ENTITY_SIZE, ENTITY_SIZE);
            if (new_entity->element_type == entity->element_type) {

                *(entity_iterator->offset_) = (i + 1) * ENTITY_SIZE;
                *(entity_iterator->read_block_) = entity_page_header->block_number;

                entity_iterator->page_header = entity_page_header;
                entity_iterator->entity = new_entity;
                entity_iterator->body = entity_body;

                *(iterator->read_block_) = entity_iterator->entity->first_block;
                *(iterator->offset_) = 0;
                iterator->entity = entity;
                
                return entity_has_next(entity_iterator);
            }
        }
        page_num = entity_page_header->next_block;
        *(entity_iterator->offset_) = 0;
        set_pointer_offset_file(cursor->file, page_num * PAGE_SIZE);

        read_from_file(cursor->file, entity_page_header, PAGE_HEADER_SIZE);        
        read_from_file(cursor->file, entity_body, PAGE_BODY_SIZE);
    } while (page_num != 0);

    return false;
}

void free_iter(Iterator* iterator) {
    free(iterator->body);
    free(iterator->element);
    free(iterator->offset_);
    free(iterator->read_block_);
    free(iterator->entity);
    free(iterator->page_header);
    free(iterator->function_helper);
    free(iterator);
}

void free_entity_iter(EntityIterator* entity_iterator) {
    free(entity_iterator->body);
    free(entity_iterator->offset_);
    free(entity_iterator->read_block_);
    free(entity_iterator->entity);
    free(entity_iterator->page_header);
    free_iter(entity_iterator->iterator);
    free(entity_iterator);
}

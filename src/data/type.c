#include "../../include/data/type.h"


void print_type(Entity* entity) {
    println("Print");
    println("Type - %i", entity->type);
    println("First - %i", entity->first_block);
    println("Last - %i", entity->last_block);
    println("Next Id - %i", entity->next_id);
    println("Name - %s", entity->type);
}

void save_entity_and_create_new(Cursor* cursor) {
    cursor->number_of_pages++;
    cursor->last_entity_block = cursor->number_of_pages;
    cursor->page->page_header->next_block = cursor->last_entity_block;
    
    flush_page(cursor);
    
    free(cursor->page->page_header);
    free(cursor->page->body);
    free(cursor->page);

    PageHeader* page_header = (PageHeader*) malloc(PAGE_HEADER_SIZE);
    Page* page = (Page*) malloc(PAGE_SIZE);
    
    page_header->block_number = cursor->last_entity_block;
    page->page_header = page_header;
    page->body = malloc(PAGE_BODY_SIZE);;
    cursor->page = page;
}

void create_page_for_type(Cursor* cursor) {
    void* page_for_type = malloc(PAGE_SIZE);
    PageHeader* page_header = (PageHeader*) malloc(PAGE_HEADER_SIZE);

    cursor->number_of_pages++;
    page_header->block_number = cursor->number_of_pages;
    
    memcpy(page_for_type, page_header, PAGE_HEADER_SIZE);
    
    set_pointer_offset_file(cursor->file, cursor->number_of_pages * PAGE_SIZE);
    write_to_file(cursor->file, page_for_type, PAGE_SIZE);

    free(page_header);
    free(page_for_type);
}

Entity* get_entity(Cursor* cursor, TypeOfElement element_type, const char* name, uint64_t* pointer, Entity* entity) {
    PageHeader* page_header = (PageHeader*) malloc(PAGE_HEADER_SIZE);
    char* page_body = (char*) malloc(PAGE_BODY_SIZE);
    
    int page_num = 0;

    do {
        set_pointer_offset_file(cursor->file, page_num * PAGE_SIZE);

        read_from_file(cursor->file, page_header, PAGE_HEADER_SIZE);        
        read_from_file(cursor->file, page_body, PAGE_BODY_SIZE);

        for (uint64_t i = 0; i < (page_header->offset / ENTITY_SIZE); i++) {
            memcpy(entity, page_body + i * ENTITY_SIZE, ENTITY_SIZE);
            if (strcmp(entity->type, name) == 0 && element_type == entity->element_type) {
                *pointer = page_num * PAGE_SIZE + i * ENTITY_SIZE + PAGE_HEADER_SIZE;
                free(page_header);
                free(page_body);
                return entity;
            }
        }
        page_num = page_header->next_block;
    } while (page_num != 0);
    
    free(page_header);
    free(page_body);
    return NULL;
}


EntityIterator* get_entity_iterator(Cursor* cursor, TypeOfElement element_type) {

    Entity* entity = malloc(ENTITY_SIZE);
    PageHeader* page_header = (PageHeader*) malloc(PAGE_HEADER_SIZE);
    char* page_body = (char*) malloc(PAGE_BODY_SIZE);
    
    uint64_t* offset_ = malloc(sizeof(uint64_t));
    uint32_t* read_block_ = malloc(UINT32_T_SIZE);
    EntityIterator* entity_iterator = (EntityIterator*) malloc(sizeof(EntityIterator));
    
    int page_num = 0;

    do {
        set_pointer_offset_file(cursor->file, page_num * PAGE_SIZE);

        read_from_file(cursor->file, page_header, PAGE_HEADER_SIZE);        
        read_from_file(cursor->file, page_body, PAGE_BODY_SIZE);

        for (uint64_t i = 0; i < (page_header->offset / ENTITY_SIZE); i++) {
            memcpy(entity, page_body + i * ENTITY_SIZE, ENTITY_SIZE);
            if (element_type == entity->element_type) {

                *offset_ = (i + 1) * ENTITY_SIZE;
                *read_block_ = page_header->block_number;

                entity_iterator->cursor = cursor;
                entity_iterator->page_header = page_header;
                entity_iterator->entity = entity;
                entity_iterator->body = page_body;
                entity_iterator->read_block_ = read_block_;
                entity_iterator->offset_ = offset_;
                
                return entity_iterator;
            }
        }
        page_num = page_header->next_block;
    } while (page_num != 0);
    
    free(page_header);
    free(page_body);
    free(entity);

    free(offset_);
    free(read_block_);
    free(entity_iterator);

    return NULL;
}

bool create_type(Cursor* cursor, Entity* entity) {
    uint64_t pointer = 0;
    Entity* table = (Entity*) malloc(ENTITY_SIZE);
    table = get_entity(cursor, entity->element_type, entity->type, &(pointer), table);
    if (table != NULL) {
        println("Entity %s with type %i already exists", entity->type, entity->element_type);
        return false;
    }

    if (cursor->page->page_header->offset + ENTITY_SIZE > PAGE_BODY_SIZE) {
        save_entity_and_create_new(cursor);
    }

    create_page_for_type(cursor);
    
    entity->next_id = 0;
    entity->first_block = cursor->number_of_pages;
    entity->last_block = cursor->number_of_pages;
    
    memcpy(cursor->page->body + cursor->page->page_header->offset, entity, ENTITY_SIZE);
    cursor->page->page_header->offset += ENTITY_SIZE;
    
    flush_page(cursor);
    
    free(table);
    return true;
}

bool delete_type(Cursor* cursor, Entity* entity) {
    uint64_t* pointer = (uint64_t*) malloc(sizeof(uint64_t));
    *pointer = 0;
    PageHeader* page_header = (PageHeader*) malloc(PAGE_HEADER_SIZE);
    Entity* delete_table = (Entity*) malloc(ENTITY_SIZE);
    delete_table = get_entity(cursor, entity->element_type, entity->type, pointer, delete_table);

    if (delete_table == NULL) {
        println("Error to find Entity with name %s", entity->type);
        return false;
    }

    uint32_t counter = 0;
    uint32_t *stack = find_all_blocks_to_delete(cursor, &(counter), delete_table);

    remove_blocks(cursor, counter, stack);
    uint64_t new_offset = erase_entity(cursor, pointer);

    if (new_offset == 0) {
        page_header = move_blocks_higher(cursor, pointer, page_header);
        remove_emtpy_blocks(cursor, page_header);
    }
    free(pointer);
    free(stack);
    free(page_header);
    free(delete_table);
    return true;
}

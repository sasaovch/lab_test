#include "../../include/io/io.h"
#include "../../include/data/constants.h"
#include <stdlib.h>

uint32_t* find_all_blocks_to_delete(Cursor* cursor, uint32_t* counter, Entity* entity) {
    uint32_t max_number = 1000;
    uint32_t* stack = (uint32_t*) malloc(max_number * UINT32_T_SIZE);
    PageHeader* page_header = (PageHeader*) malloc(PAGE_HEADER_SIZE);
    
    uint32_t read_block = entity->first_block;
    *counter = 0;
    stack[*counter] = read_block;
    
    while (read_block != 0) {
        uint64_t page_offset = read_block * PAGE_SIZE;
        set_pointer_offset_file(cursor->file, page_offset);
        read_from_file(cursor->file, page_header, PAGE_HEADER_SIZE);  

        read_block = page_header->next_block;
        stack[++(*counter)] = read_block;

        if (*counter >= max_number) {
            max_number += 1000;
            stack = (uint32_t*) realloc(stack, max_number * UINT32_T_SIZE);
        }
    }
    free(page_header);
    return stack;
}

void remove_blocks(Cursor* cursor, uint32_t counter, uint32_t* stack) {
    uint32_t zero = 0;
    uint32_t minus_one = -1;
    void* empty_body = calloc(1, PAGE_BODY_SIZE);
    
    while (--counter != minus_one) {
        uint32_t block_number = stack[counter];
        uint64_t page_offset = block_number * PAGE_SIZE;

        set_pointer_offset_file(cursor->file, page_offset + PAGE_HEADER_SIZE - UINT32_T_SIZE);
        write_to_file(cursor->file, &(zero), UINT32_T_SIZE);
        write_to_file(cursor->file, empty_body, PAGE_BODY_SIZE);
        
        if (block_number == cursor->number_of_pages) {
            cursor->number_of_pages--;
        }
    }
    free(empty_body);

    // int result = ftruncate(cursor->file->file_descriptor, (cursor->number_of_pages + 1) * PAGE_SIZE);
    // error_exit(result, "Failed to clear the file in remove_blocks()");
}

uint64_t erase_entity(Cursor* cursor, uint64_t* pointer) {
    uint64_t page_number = *pointer / PAGE_SIZE;
    uint64_t offset = *pointer % PAGE_SIZE - PAGE_HEADER_SIZE;
    
    PageHeader* page_header = (PageHeader*) malloc(PAGE_HEADER_SIZE);

    set_pointer_offset_file(cursor->file, page_number * PAGE_SIZE);
    read_from_file(cursor->file, page_header, PAGE_HEADER_SIZE);  

    char* old_body = (char*) malloc(PAGE_BODY_SIZE);
    char* new_body = (char*) malloc(PAGE_BODY_SIZE);

    read_from_file(cursor->file, old_body, PAGE_BODY_SIZE);

    memcpy(new_body, old_body, offset);
    memcpy(new_body + offset, old_body + offset + ENTITY_SIZE, PAGE_BODY_SIZE - offset - ENTITY_SIZE);

    uint64_t new_offset = page_header->offset - ENTITY_SIZE;

    set_pointer_offset_file(cursor->file, page_number * PAGE_SIZE + PAGE_HEADER_SIZE - UINT32_T_SIZE);
    write_to_file(cursor->file, &(new_offset), UINT32_T_SIZE);
    write_to_file(cursor->file, new_body, PAGE_BODY_SIZE);

    free(page_header);
    free(old_body);
    free(new_body);

    return new_offset;
}

void remove_emtpy_blocks(Cursor* cursor, PageHeader* page_header) {
    void* empty_block = calloc(1, PAGE_BODY_SIZE);
    uint32_t zero = 0;

    set_pointer_offset_file(cursor->file, page_header->block_number * PAGE_SIZE + PAGE_HEADER_SIZE - UINT32_T_SIZE);
    write_to_file(cursor->file, &(zero), UINT32_T_SIZE);
    write_to_file(cursor->file, empty_block, PAGE_BODY_SIZE);

    if (page_header->block_number == cursor->number_of_pages) {
        page_header->offset = 0;
        while (page_header->offset == 0) {
            cursor->number_of_pages--;
            set_pointer_offset_file(cursor->file, cursor->number_of_pages * PAGE_SIZE);
            read_from_file(cursor->file, page_header, PAGE_HEADER_SIZE);
        }
        // int result = ftruncate(cursor->file->file_descriptor, (cursor->number_of_pages + 1) * PAGE_SIZE);
        // error_exit(result, "Failed to clear the file in fill_block_with_zero()");
    }
    free(empty_block);
}

PageHeader* move_blocks_higher(Cursor* cursor, uint64_t* pointer, PageHeader* old_header) {
    uint64_t page_number = *pointer / PAGE_SIZE;
    uint32_t zero = ZERO;
    
    set_pointer_offset_file(cursor->file, page_number * PAGE_SIZE);
    read_from_file(cursor->file, old_header, PAGE_HEADER_SIZE);  

    PageHeader* page_header = (PageHeader*) malloc(PAGE_HEADER_SIZE);
    void* copy_body = malloc(PAGE_BODY_SIZE);
    
    while (old_header->next_block != 0) {
        set_pointer_offset_file(cursor->file, old_header->next_block * PAGE_SIZE);
        read_from_file(cursor->file, page_header, PAGE_HEADER_SIZE);
        read_from_file(cursor->file, copy_body, PAGE_BODY_SIZE);
        
        if (page_header->next_block == 0) {
            set_pointer_offset_file(cursor->file, old_header->block_number * PAGE_SIZE + UINT32_T_SIZE);
            write_to_file(cursor->file, &(zero), UINT32_T_SIZE);
        } else {
            set_pointer_offset_file(cursor->file, old_header->block_number * PAGE_SIZE + PAGE_HEADER_SIZE - UINT32_T_SIZE);
        }
        
        write_to_file(cursor->file, &(page_header->offset), UINT32_T_SIZE);
        write_to_file(cursor->file, copy_body, PAGE_BODY_SIZE);

        memcpy(old_header, page_header, PAGE_HEADER_SIZE);
    }
    free(page_header);
    free(copy_body);
    return old_header;
}

void create_new_page(Cursor* cursor, PageHeader* page_header, uint64_t curr_page_offset) {
    if (page_header->next_block == 0) {
        char* empty_body = (char*) calloc(1, PAGE_BODY_SIZE);
        cursor->number_of_pages++;

        set_pointer_offset_file(cursor->file, curr_page_offset + UINT32_T_SIZE);
        write_to_file(cursor->file, &(cursor->number_of_pages), UINT32_T_SIZE);

        page_header->block_number = cursor->number_of_pages;
        page_header->offset = 0;
        page_header->next_block = 0;

        set_pointer_offset_file(cursor->file, cursor->number_of_pages * PAGE_SIZE);
        write_to_file(cursor->file, page_header, PAGE_HEADER_SIZE);
        write_to_file(cursor->file, empty_body, PAGE_BODY_SIZE);
        free(empty_body);
    } else {
        set_pointer_offset_file(cursor->file, curr_page_offset + UINT32_T_SIZE);
        write_to_file(cursor->file, &(page_header->next_block), UINT32_T_SIZE);
        set_pointer_offset_file(cursor->file, page_header->next_block * PAGE_SIZE);
        read_from_file(cursor->file, page_header, PAGE_HEADER_SIZE);
    }
}


uint32_t remove_empty_block(Cursor* cursor, PageHeader* first_header, PageHeader* second_header) {
    void* empty_block = calloc(1, PAGE_BODY_SIZE);
    void* copy_body = malloc(PAGE_BODY_SIZE);

    uint32_t last_block = first_header->block_number;
    uint32_t zero = ZERO;

    while (first_header->next_block != 0) {
        set_pointer_offset_file(cursor->file, first_header->next_block * PAGE_SIZE);
        read_from_file(cursor->file, second_header, PAGE_HEADER_SIZE);
        read_from_file(cursor->file, copy_body, PAGE_BODY_SIZE);
        
        if (second_header->next_block == 0 && cursor->number_of_pages == second_header->block_number) {
            set_pointer_offset_file(cursor->file, first_header->block_number * PAGE_SIZE + UINT32_T_SIZE);
            write_to_file(cursor->file, &(zero), UINT32_T_SIZE);
        } else {
            set_pointer_offset_file(cursor->file, first_header->block_number * PAGE_SIZE + PAGE_HEADER_SIZE - UINT32_T_SIZE);
        }

        write_to_file(cursor->file, &(second_header->offset), UINT32_T_SIZE);
        write_to_file(cursor->file, copy_body, PAGE_BODY_SIZE);
        memcpy(first_header, second_header, PAGE_HEADER_SIZE);
        if (first_header->offset != 0) last_block = first_header->block_number;
    }

    set_pointer_offset_file(cursor->file, first_header->block_number * PAGE_SIZE + PAGE_HEADER_SIZE - UINT32_T_SIZE);
    write_to_file(cursor->file, &(zero), UINT32_T_SIZE);
    write_to_file(cursor->file, empty_block, PAGE_BODY_SIZE);    

    free(empty_block);
    free(copy_body);
    return last_block;
}

uint32_t find_block_before(Cursor* cursor, const PageHeader* page_header, const Entity* entity) {
    uint32_t goal_block = page_header->block_number;
    uint32_t prev_block = goal_block;
    uint32_t curr_block = entity->first_block;
    PageHeader* curr_header = (PageHeader*) malloc(PAGE_HEADER_SIZE);
    curr_header->block_number = 0;
    while (curr_block != goal_block && curr_block != 0) {

        set_pointer_offset_file(cursor->file, curr_block * PAGE_SIZE);
        read_from_file(cursor->file, curr_header, PAGE_HEADER_SIZE);
        prev_block = curr_block;
        curr_block = curr_header->next_block;
    }
    free(curr_header);
    return prev_block;
}

void cut_blocks(Cursor* cursor, const PageHeader* page_header, const uint64_t* pointer, const Entity* entity) {
    PageHeader* old_header = (PageHeader*) malloc(PAGE_HEADER_SIZE);
    PageHeader* copy_header = (PageHeader*) malloc(PAGE_HEADER_SIZE);

    memcpy(old_header, page_header, PAGE_HEADER_SIZE);
    uint32_t last_block = remove_empty_block(cursor, old_header, copy_header);
    uint32_t pre_last_block = find_block_before(cursor, old_header, entity);

    if (old_header->block_number == cursor->number_of_pages && old_header->block_number != entity->last_block) {
        old_header->offset = 0;

        cursor->number_of_pages--;
        set_pointer_offset_file(cursor->file, *pointer + TYPE_OF_ELEMENT_SIZE + VALUE_TYPE_SIZE + UINT32_T_SIZE);
        write_to_file(cursor->file, &(pre_last_block), UINT32_T_SIZE);
        uint32_t zero = ZERO;
        set_pointer_offset_file(cursor->file, pre_last_block * PAGE_SIZE + UINT32_T_SIZE);
        write_to_file(cursor->file, &(zero), UINT32_T_SIZE);

        // int result = ftruncate(cursor->file->file_descriptor, (cursor->number_of_pages + 1) * PAGE_SIZE);
        // error_exit(result, "Failed to clear the file.\n");
    } else {
        set_pointer_offset_file(cursor->file, *pointer + TYPE_OF_ELEMENT_SIZE + VALUE_TYPE_SIZE + UINT32_T_SIZE);
        write_to_file(cursor->file, &(last_block), UINT32_T_SIZE);
    }

    free(old_header);
    free(copy_header);
}

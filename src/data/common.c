#include "../../include/include.h"
#include "data/constants.h"
#include <stdint.h>
#include <stdlib.h>

uint32_t* create_element(
    Cursor* cursor, void* element,
    TypeOfElement element_type, const char* type,
    FunctionHelper* function_helper,
    uint32_t* id
) {
    uint64_t pointer = 0;

    Entity* entity = (Entity*) malloc(ENTITY_SIZE);
    PageHeader* page_header = (PageHeader*) malloc(PAGE_HEADER_SIZE);
    uint32_t* return_id = malloc(UINT32_T_SIZE);
    
    entity = get_entity(cursor, element_type, type, &(pointer), entity);
    if (entity == NULL) {
        free(entity);
        free(page_header);
        free(return_id);
        println("Error to find Entity with name %s", type);
        return NULL;
    }

    uint32_t page_num = entity->last_block;
    uint64_t page_offset = page_num * PAGE_SIZE;

    set_pointer_offset_file(cursor->file, page_offset);
    read_from_file(cursor->file, page_header, PAGE_HEADER_SIZE);
    
    uint64_t global_offset = page_offset + page_header->offset + PAGE_HEADER_SIZE;

    if (page_header->offset + function_helper->get_size_of_element(element) > PAGE_BODY_SIZE &&
        page_header->offset != 0) {
        create_new_page(cursor, page_header, page_offset);
        
        entity->last_block = cursor->number_of_pages;
        global_offset = cursor->number_of_pages * PAGE_SIZE + PAGE_HEADER_SIZE;
    }

    if (id == NULL) {
        function_helper->work_with_id(element, entity->next_id, true);
        *return_id = entity->next_id;
        entity->next_id++;
    } else {
        function_helper->work_with_id(element, *id, true);
        *return_id = *id;
    }

    set_pointer_offset_file(cursor->file, global_offset);
    function_helper->write_element_to_file(cursor, page_header, entity, element);

    page_offset = page_header->block_number * PAGE_SIZE;
    
    set_pointer_offset_file(cursor->file, page_offset + PAGE_HEADER_SIZE - UINT32_T_SIZE);
    write_to_file(cursor->file, &(page_header->offset), UINT32_T_SIZE);
    
    if (function_helper->get_size_of_element(element) > PAGE_BODY_SIZE) {
        create_new_page(cursor, page_header, page_offset);
        entity->last_block = cursor->number_of_pages;
    }

    set_pointer_offset_file(cursor->file, pointer + TYPE_OF_ELEMENT_SIZE + VALUE_TYPE_SIZE + UINT32_T_SIZE);
    write_to_file(cursor->file, &(entity->last_block), UINT32_T_SIZE);
    write_to_file(cursor->file, &(entity->next_id), UINT32_T_SIZE);

    free(entity);
    free(page_header);

    return return_id;
}

void* get_elements_by_condition(
        Cursor* cursor, Entity* entity, 
        uint64_t size_of_element_malloc, 
        void* helper, 
        FunctionHelper* function_helper
    ) {
    uint32_t read_block = entity->first_block;
    uint64_t stack_offset = UINT32_T_SIZE;
    uint32_t find_number = 0;
    uint64_t stack_size = PAGE_BODY_SIZE;

    PageHeader* page_header = (PageHeader*) malloc(PAGE_HEADER_SIZE);
    char* body = (char*) malloc(PAGE_BODY_SIZE);
    void* stack = malloc(PAGE_BODY_SIZE);
    void* element = (void*) malloc(size_of_element_malloc);

    while (read_block != 0) {
        uint64_t offset = 0;
        uint64_t page_offset = read_block * PAGE_SIZE;

        set_pointer_offset_file(cursor->file, page_offset);
        read_from_file(cursor->file, page_header, PAGE_HEADER_SIZE);  
        read_from_file(cursor->file, body, PAGE_BODY_SIZE);

        while (offset < page_header->offset) {

            element = function_helper->read_big_element(cursor, page_header, element, &(offset), body, &(read_block));
            if (function_helper->condition(element, helper)) {
                find_number++;
            
                if (stack_offset + function_helper->get_size_of_element(element) >= stack_size) {
                    stack_size += (function_helper->get_size_of_element(element) / PAGE_BODY_SIZE + 1) * PAGE_BODY_SIZE;
                    stack = realloc(stack, stack_size);
                }
                function_helper->memcpy_element(element, stack, &(stack_offset));
            }
            offset = (offset + function_helper->get_size_of_element(element)) % PAGE_BODY_SIZE;
        }
        read_block = page_header->next_block;
    }
    memcpy(stack, &(find_number), UINT32_T_SIZE);

    free(element);
    free(page_header);
    free(body);
    
    return stack;
}

Iterator* select_element(
        Cursor* cursor,
        TypeOfElement element_type, const char* type,
        uint64_t size_of_element_malloc, 
        void* helper, 
        FunctionHelper* function_helper
    ) {
    uint64_t pointer = 0;

    Entity* entity = (Entity*) malloc(ENTITY_SIZE);
    PageHeader* page_header = (PageHeader*) malloc(PAGE_HEADER_SIZE);
    
    entity = get_entity(cursor, element_type, type, &(pointer), entity);
    if (entity == NULL) {
        free(entity);
        free(page_header);
        println("Error to find Entity with name %s", type);
        return NULL;
    }

    char* body = (char*) malloc(PAGE_BODY_SIZE);
    void* element = (void*) malloc(size_of_element_malloc);
    
    uint64_t* offset_ = malloc(sizeof(uint64_t));
    *offset_ = 0;
    uint32_t* read_block_ = malloc(UINT32_T_SIZE);
    *read_block_ = entity->first_block;;

    Iterator* iterator = (Iterator*) malloc(sizeof(Iterator));

    iterator->cursor = cursor;
    iterator->page_header = page_header;
    iterator->entity = entity;
    iterator->body = body;
    iterator->read_block_ = read_block_;
    iterator->offset_ = offset_;
    iterator->element = element;
    iterator->helper = helper;
    iterator->function_helper = function_helper;
    
    return iterator;
}

void* find_element(
        Cursor* cursor, Entity* entity, 
        Page* page, uint64_t size_of_element_malloc, 
        void* find_elem, uint64_t* offset_,
        FunctionHelper* function_helper
    ) {
    PageHeader* page_header = (PageHeader*) malloc(PAGE_HEADER_SIZE);

    PageHeader* const_page_header = page->page_header;
    uint32_t read_block = entity->first_block;
    
    void* body = page->body;
    void* element = (void*) malloc(size_of_element_malloc);

    while (read_block != 0) {
        uint64_t offset = 0;
        uint64_t page_offset = read_block * PAGE_SIZE;
        
        set_pointer_offset_file(cursor->file, page_offset);
        read_from_file(cursor->file, page_header, PAGE_HEADER_SIZE);  
        read_from_file(cursor->file, body, PAGE_BODY_SIZE);
        memcpy(const_page_header, page_header, PAGE_HEADER_SIZE);

        while (offset < page_header->offset) {
            element = function_helper->read_big_element(cursor, page_header, element, &(offset), body, &(read_block));

            if (function_helper->condition(element, find_elem)) {
                page->body = body;
                page->page_header = const_page_header;
                *offset_ = offset;
                free(page_header);
                return element;
            }
            offset = (offset + function_helper->get_size_of_element(element)) % PAGE_BODY_SIZE;
        }
        read_block = page_header->next_block;
    }
    free(element);
    free(page_header);
    return NULL;
}

void remove_bid_element(
    Cursor* cursor, PageHeader* page_header, 
    const uint64_t* pointer, uint64_t (*get_size_of_element)(void *), void* element
    ) {
    uint32_t page_numbers = get_size_of_element(element) / PAGE_BODY_SIZE;

    if (get_size_of_element(element) % PAGE_BODY_SIZE) {
        page_numbers++;
    }
    uint32_t statck_pointer = 0;
    uint32_t size_of_deleted = page_numbers * 2;
    uint32_t* stack = (uint32_t*) calloc(page_numbers, UINT32_T_SIZE);
    uint32_t deleted_pointer = 0;
    uint32_t* deleted_blocks = (uint32_t*) calloc(size_of_deleted, UINT32_T_SIZE);
    PageHeader* next_page_header = (PageHeader*) malloc(PAGE_HEADER_SIZE);
    for (uint32_t i = 0; i < page_numbers; i++) {
        stack[i] = page_header->block_number;
        deleted_blocks[deleted_pointer] = page_header->block_number;
        deleted_pointer++;
        if (i == page_numbers - 1) break;
        set_pointer_offset_file(cursor->file, page_header->next_block * PAGE_SIZE);
        read_from_file(cursor->file, page_header, PAGE_HEADER_SIZE);
    }

    void* empty_block = calloc(1, PAGE_BODY_SIZE);
    void* copy_body = malloc(PAGE_BODY_SIZE);

    while (page_header->next_block != 0 && page_header->offset != 0) {
        set_pointer_offset_file(cursor->file, page_header->next_block * PAGE_SIZE);
        read_from_file(cursor->file, next_page_header, PAGE_HEADER_SIZE);
        read_from_file(cursor->file, copy_body, PAGE_BODY_SIZE);

        set_pointer_offset_file(cursor->file, stack[statck_pointer] * PAGE_SIZE + PAGE_HEADER_SIZE - UINT32_T_SIZE);

        write_to_file(cursor->file, &(next_page_header->offset), UINT32_T_SIZE);
        write_to_file(cursor->file, copy_body, PAGE_BODY_SIZE);
        stack[statck_pointer] = next_page_header->block_number;
        
        if (deleted_pointer == size_of_deleted - 1) {
            size_of_deleted *= 2;
            deleted_blocks = realloc(deleted_blocks, size_of_deleted * UINT32_T_SIZE);
        }
    
        deleted_blocks[deleted_pointer] = next_page_header->block_number;
        deleted_pointer++;
        memcpy(page_header, next_page_header, PAGE_HEADER_SIZE);
        statck_pointer = (statck_pointer + 1) % page_numbers;
    }
    uint32_t zero = ZERO;
    set_pointer_offset_file(cursor->file, page_header->block_number * PAGE_SIZE + PAGE_HEADER_SIZE - UINT32_T_SIZE);
    write_to_file(cursor->file, &(zero), UINT32_T_SIZE);
    write_to_file(cursor->file, empty_block, PAGE_BODY_SIZE);  

    //последний блок, который не нужно отрезать. delete_pointer указывает на следующую ячейку для записи, предпоследний блок 0
    uint32_t blocks_to_cut = deleted_pointer - 1;
    //самые нижние блоки теперь считаем пустыми
    uint32_t blocks_to_empty = blocks_to_cut - page_numbers;
    bool is_cut = false;

    for (uint32_t i = 0;  i < page_numbers; i++) {
        if (deleted_blocks[blocks_to_cut] == cursor->number_of_pages) {
            cursor->number_of_pages--;
            blocks_to_cut--;
            is_cut = true;
        } else {
            break;
        }
    }

    set_pointer_offset_file(cursor->file, *pointer + TYPE_OF_ELEMENT_SIZE + VALUE_TYPE_SIZE + UINT32_T_SIZE);
    write_to_file(cursor->file, &(deleted_blocks[blocks_to_empty]), UINT32_T_SIZE);
    if (is_cut) {
        // int result = ftruncate(cursor->file->file_descriptor, (cursor->number_of_pages + 1) * PAGE_SIZE);
        // error_exit(result, "Failed to clear the file.\n");
        set_pointer_offset_file(cursor->file, deleted_blocks[blocks_to_empty] * PAGE_SIZE + UINT32_T_SIZE);
        write_to_file(cursor->file, &(zero), UINT32_T_SIZE);
    }

    while (blocks_to_empty < blocks_to_cut) {
        set_pointer_offset_file(cursor->file, deleted_blocks[++blocks_to_empty] * PAGE_SIZE + PAGE_HEADER_SIZE - UINT32_T_SIZE);
        write_to_file(cursor->file, &(zero), UINT32_T_SIZE);
        write_to_file(cursor->file, empty_block, PAGE_BODY_SIZE);
    }

    free(empty_block);
    free(copy_body);
    free(next_page_header);
    free(stack);
    free(deleted_blocks);
}

bool delete_element(
        Cursor* cursor, void* element, 
        uint64_t size_of_sturcture, 
        void* type, TypeOfElement element_type, 
        FunctionHelper* function_helper
    ) {
    uint64_t pointer = 0;
    uint64_t offset = 0;

    Entity* entity = (Entity*) malloc(ENTITY_SIZE);
    Page* page = (Page*) malloc(sizeof(Page));
    PageHeader* page_header = (PageHeader*) malloc(PAGE_HEADER_SIZE);
    
    entity = get_entity(cursor, element_type, type, &(pointer), entity);
    if (entity == NULL) {
        println("Error to find Node with name %s", type);
        free(entity);
        free(page_header);
        free(page);
        return false;
    }

    char* new_body = (char*) calloc(1, PAGE_BODY_SIZE);
    void* body = malloc(PAGE_BODY_SIZE);

    page->page_header = page_header;
    page->body = body;

    void* find_el = find_element(cursor, entity, page, size_of_sturcture, element, &(offset),  function_helper);
    if (find_el == NULL) {
        free(page->page_header);
        free(page->body);
        free(page);
        free(entity);
        free(new_body);
        return false;
    }
    while (find_el != NULL) {
        uint64_t size_of_element = function_helper->get_size_of_element(find_el);
        
        if (size_of_element <= PAGE_BODY_SIZE) {
            memcpy(new_body, page->body, offset);
            memcpy(new_body + offset, page->body + offset + size_of_element, PAGE_BODY_SIZE - offset - size_of_element);

            uint64_t new_offset = page->page_header->offset - size_of_element;

            set_pointer_offset_file(cursor->file, page->page_header->block_number * PAGE_SIZE + PAGE_HEADER_SIZE - UINT32_T_SIZE);
            write_to_file(cursor->file, &(new_offset), UINT32_T_SIZE);
            write_to_file(cursor->file, new_body, PAGE_BODY_SIZE);
            
            if (new_offset == 0) {
                cut_blocks(cursor, page->page_header, &(pointer), entity);
            }
        } else {
            remove_bid_element(cursor, page_header, &(pointer), function_helper->get_size_of_element, find_el);
        }
        free(find_el);
        find_el = find_element(cursor, entity, page, size_of_sturcture, element, &(offset),  function_helper);
    }

    free(page->page_header);
    free(page->body);
    free(page);
    free(entity);
    free(new_body);
    return true;
}

bool update_element(
        Cursor* cursor, void* old_element, 
        void* new_element,
        uint64_t size_of_sturcture, 
        void *type, TypeOfElement element_type, 
        FunctionHelper* function_helper
    ) {
    uint64_t pointer = 0;
    uint64_t offset = 0;

    Entity* entity = (Entity*) malloc(ENTITY_SIZE);
    Page* page = (Page*) malloc(sizeof(Page));
    PageHeader* page_header = (PageHeader*) malloc(PAGE_HEADER_SIZE);
    

    entity = get_entity(cursor, element_type, type, &(pointer), entity);
    if (entity == NULL) {
        println("Error to find Node with name %s", type);
        free(entity);
        free(page_header);
        free(page);
        return false;
    }

    char* new_body = (char*) calloc(1, PAGE_BODY_SIZE);
    void* body = malloc(PAGE_BODY_SIZE);

    page->page_header = page_header;
    page->body = body;

    void* find_el = find_element(cursor, entity, page, size_of_sturcture, old_element, &(offset),  function_helper);
    if (find_el == NULL) {
        free(page->page_header);
        free(page->body);
        free(page);
        free(entity);
        free(new_body);
        return false;
    }
    uint32_t size = 256;
    uint32_t stack_pointer = 0;
    uint32_t* stack_ids = malloc(UINT32_T_SIZE * size);
    while (find_el != NULL) {
        uint64_t size_of_element = function_helper->get_size_of_element(find_el);
        
        if (size_of_element <= PAGE_BODY_SIZE) {
            memcpy(new_body, page->body, offset);
            memcpy(new_body + offset, page->body + offset + size_of_element, PAGE_BODY_SIZE - offset - size_of_element);

            uint64_t new_offset = page->page_header->offset - size_of_element;
            
            set_pointer_offset_file(cursor->file, page->page_header->block_number * PAGE_SIZE + PAGE_HEADER_SIZE - UINT32_T_SIZE);
            write_to_file(cursor->file, &(new_offset), UINT32_T_SIZE);
            write_to_file(cursor->file, new_body, PAGE_BODY_SIZE);
            
            if (new_offset == 0) {
                cut_blocks(cursor, page->page_header, &(pointer), entity);
            }
        } else {
            remove_bid_element(cursor, page_header, &(pointer), function_helper->get_size_of_element, find_el);
        }
        uint32_t id = function_helper->work_with_id(find_el, 0, false);
        
        stack_ids[stack_pointer++] = id;
        if (stack_pointer == size) {
            size += 256;
            stack_ids = realloc(stack_ids, UINT32_T_SIZE * size);
        }
        free(find_el);
        find_el = find_element(cursor, entity, page, size_of_sturcture, old_element, &(offset),  function_helper);

    }

    for (uint32_t i = 0; i < stack_pointer; i++) {
        uint32_t id = stack_ids[i];
        uint32_t* new_id = create_element(cursor, new_element, element_type, type, function_helper, &(id));
        if (new_id == NULL) {
            free(new_id);
            free(page->page_header);
            free(page->body);
            free(page);
            free(entity);
            free(new_body);
            free(stack_ids);
            return false;
        }
        free(new_id);
    }

    free(page->page_header);
    free(page->body);
    free(page);
    free(entity);
    free(new_body);
    free(stack_ids);
    return true;
}

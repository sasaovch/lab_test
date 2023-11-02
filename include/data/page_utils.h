#ifndef LAB_1_UTILS_H
#define LAB_1_UTILS_H

#include "../io/io.h"


uint32_t* find_all_blocks_to_delete(Cursor* cursor, uint32_t* counter, Entity* entity);

void remove_blocks(Cursor* cursor, uint32_t counter, uint32_t* stack);

uint64_t erase_entity(Cursor* cursor, uint64_t* pointer);

void remove_emtpy_blocks(Cursor* cursor, PageHeader* page_header);

PageHeader* move_blocks_higher(Cursor* cursor, uint64_t* pointer, PageHeader* old_header);

void create_new_page(Cursor* cursor, PageHeader* page_header, uint64_t curr_table_page_offset);

uint32_t remove_empty_block(Cursor* cursor, PageHeader* first_header, PageHeader* second_header);

uint32_t find_block_before(Cursor* cursor, PageHeader* page_header, Entity* entity);

void cut_blocks(Cursor* cursor, PageHeader* page_header, uint64_t* pointer, Entity* entity);

#endif

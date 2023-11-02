#include "../../include/data/page.h"


void print_page(PageHeader* page_header) {
    println("Header");
    println("Block - %i", page_header->block_number);
    println("Next - %i", page_header->next_block);
    println("Offset - %i", page_header->offset);
}

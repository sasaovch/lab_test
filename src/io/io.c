#include "../../include/io/io.h"
#include "../../include/data/constants.h"
#include <stdint.h>


void println(const char *line, ...) {
    va_list args;
    va_start(args, line);
    vprintf(line, args);
    printf("\n");
    va_end(args);
}

void error_exit(int var, const char *line, ...) {
    if (var == -1) {
        va_list args;
        va_start(args, line);
        vprintf(line, args);
        printf("\n");
        va_end(args);
        exit(EXIT_FAILURE);
    }
}

void debug(int num, const char *line, ...) {
    println("Debug %i", num);
    va_list args;
    va_start(args, line);
    vprintf(line, args);
    printf("\n");
    va_end(args);
}

void read_from_file(File* file, void* read_buf, uint64_t size) {
    uint32_t bytes_read = fread(read_buf, size, 1, file->file);
    error_exit(bytes_read, "Error reading from file");
}

void fail_print(const char *line, ...) {
    println("---------------------------");
    va_list args;
    va_start(args, line);
    vprintf(line, args);
    printf("\n");
    va_end(args);
    println("---------------------------");
}

uint32_t find_last_entity(uint64_t file_length, Page* page, File* file) {
    if (file_length == 0) {
        return 0;
    }

    uint32_t last_block_number = 0;
    do {
        last_block_number = page->page_header->next_block;
        fseek(file->file, page->page_header->next_block * PAGE_SIZE, SEEK_SET);
        read_from_file(file, page->page_header, PAGE_HEADER_SIZE);
    } while (page->page_header->next_block != 0);

    read_from_file(file, page->body, PAGE_BODY_SIZE);
    return last_block_number;
}

Cursor* db_open(const char* filename) {
    bool file_exists = access(filename, F_OK) == 0;
    // try to read the file
    if (!file_exists) {
        println("File %s doesn't exist", filename);

        FILE* f = fopen(filename, "w");
        if (f == NULL) {
            println("Can't create file");
        }
        error_exit(fclose(f), "Error while closing file");
    }
    FILE* f = fopen(filename, "rb+");
    if (f == NULL) {
        error_exit(-1, "Unabled to open file");
    }

    uint64_t file_length = fseek(f, 0, SEEK_END);
    error_exit(file_length, "Failed to find end of file");
    
    File* file = (File*) malloc(sizeof(File));
    PageHeader* pageHeader = (PageHeader*) malloc(sizeof(PageHeader));
    Page* page = (Page*) malloc(sizeof(Page));
    Cursor* cursor = (Cursor*) malloc(sizeof(Cursor));

    void* page_body = malloc(PAGE_BODY_SIZE);

    file->file = f;
    file->file_length = file_length;

    page->body = page_body;
    page->page_header = pageHeader;

    cursor->file = file;
    cursor->last_entity_block = find_last_entity(file_length, page, file);
    if (file_length == 0) {
        cursor->number_of_pages = 0;
    } else {
        cursor->number_of_pages = (file_length - 1) / PAGE_SIZE;
    }
    cursor->page = page;

    return cursor;
}

void set_pointer_offset_file(File* file, uint64_t offset) {
    uint64_t offs = fseek(file->file, offset, SEEK_SET);
    error_exit(offs, "Error seeking offset");
}

void write_to_file(File* file, const void* write_buf, uint64_t size) {
    uint32_t bytes_written = fwrite(write_buf, size, 1, file->file);
    error_exit(bytes_written, "Error writing to file");
}

uint64_t get_page_offset(Cursor* cursor) {
    return cursor->page->page_header->block_number * PAGE_SIZE;
}

void flush_page(Cursor* cursor) {
    set_pointer_offset_file(cursor->file, get_page_offset(cursor));
    write_to_file(cursor->file, cursor->page->page_header, PAGE_HEADER_SIZE);
    write_to_file(cursor->file, cursor->page->body, PAGE_BODY_SIZE);
}

void db_close(Cursor* cursor) {
    fclose(cursor->file->file);
    free(cursor->file);
    free(cursor->page->page_header);
    free(cursor->page->body);
    free(cursor->page);
    free(cursor);
}

void write_uint_32_to_file(Cursor* cursor, uint32_t number) {
    write_to_file(cursor->file, &(number), UINT32_T_SIZE);
}
void write_type_to_file(Cursor* cursor, char* type) {
    write_to_file(cursor->file, type, NAME_TYPE_LENGTH + 1);
}
void write_string_to_file(Cursor* cursor, char* string, uint32_t length) {
    write_to_file(cursor->file, string, CHAR_SIZE * length);
}

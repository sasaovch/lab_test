#ifndef LAB_1_IO
#define LAB_1_IO

// #include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
// #include <unistd.h>
// #include <fcntl.h>

#include "../data/data.h"
#include "../data/util_data.h"

void println(const char *line, ...);

void error_exit(int var, const char *line, ...);

void debug(int num, const char *line, ...);

void read_from_file(File* file, void* read_buf, uint64_t size);

uint32_t find_last_entity(uint64_t file_length, Page* page, File* file);

Cursor* db_open(const char* filename);

void set_pointer_offset_file(File* file, uint64_t offset);

void write_to_file(File* file, const void* write_buf, uint64_t size);

uint64_t get_page_offset(Cursor* cursor);

void flush_page(Cursor* cursor);

void db_close(Cursor* cursor);

void write_uint_32_to_file(Cursor* cursor, uint32_t number);

void write_type_to_file(Cursor* cursor, char* type);

void write_string_to_file(Cursor* cursor, char* string, uint32_t length);

void fail_print(const char *line, ...);


#endif

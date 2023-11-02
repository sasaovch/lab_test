#include <gmock/gmock-matchers.h>
#include "gtest/gtest.h"

extern "C" {
    #include "test_data.h"
}

TEST(test_page, page_add_item) {
    char* filename = "data";
    Cursor* cursor = db_open(filename);
    
    create_test(cursor);
    delete_test(cursor);
    update_test(cursor);
    select_test(cursor);
    
    db_close(cursor);
}
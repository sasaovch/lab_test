#include "../include/test_data.h"
#include "data/constants.h"


int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Not enough parameters\n");
        exit(EXIT_FAILURE);
    }
    char* filename = argv[1];
    Cursor* cursor = db_open(filename);
    
    create_test(cursor);
    delete_test(cursor);
    update_test(cursor);
    select_test(cursor);
    
    db_close(cursor);
    return 0;
}

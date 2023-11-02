#ifndef LAB_1_TEST_H
#define LAB_1_TEST_H

#include "include.h"

void create_entity(Cursor* cursor);
void create_entity_1(Cursor* cursor);
void create_nodes(Cursor* cursor);
void create_nodes_1(Cursor* cursor);
void create_relationships(Cursor* cursor);
void create_relationships_1(Cursor* cursor);
void create_properties(Cursor* cursor);
void delete_entity(Cursor* cursor);
void delete_nodes(Cursor* cursor);
void delete_relationships(Cursor* cursor);
void delete_properties(Cursor* cursor);
void update_entity(Cursor* cursor);
void update_nodes(Cursor* cursor);
void update_relationships(Cursor* cursor);
void update_properties(Cursor* cursor);
void select_node(Cursor* cursor);
void select_properties_by_node_t(Cursor* cursor);
void select_relationships_by_node_t(Cursor* cursor);

void create_test(Cursor* cursor);
void update_test(Cursor* cursor);
void delete_test(Cursor* cursor);
void select_test(Cursor* cursor);

void select_smoke_test(Cursor* cursor);
void update_smoke_test(Cursor* cursor);
void delete_smoke_test(Cursor* cursor);
void select_delete_smoke_test(Cursor* cursor);

void create_n_nodes_(Cursor* cursor, int n);

#endif

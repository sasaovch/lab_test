#ifndef LAB_1_CRUD_H
#define LAB_1_CRUD_H

#include "common.h"
#include "data/data.h"

uint32_t* create_node(Cursor* cursor, Node* node);

uint32_t* create_relationship(Cursor* cursor, Relationship* relationship);

uint32_t* create_property(Cursor* cursor, Property* property);

bool delete_all_nodes(Cursor* cursor, Node* node);

bool delete_node_by_id(Cursor* cursor, Node* node);

bool delete_node_by_name(Cursor* cursor, Node* node);

bool delete_nodes_greater_id(Cursor* cursor, Node* node);

bool delete_all_relationships(Cursor* cursor, Relationship* relationship);

bool delete_relationship_by_id(Cursor* cursor, Relationship* relationship);

bool delete_relationship_by_parent(Cursor* cursor, Relationship* relationship);

bool delete_relationship_by_child(Cursor* cursor, Relationship* relationship);

bool delete_all_properties(Cursor* cursor, Property* property);

bool delete_property_by_subject(Cursor* cursor, Property* property);

bool delete_property_by_key(Cursor* cursor, Property* property);

bool update_all_nodes(Cursor* cursor, Node* old_node, Node* new_node);

bool update_node_by_id(Cursor* cursor, Node* old_node, Node* new_node);

bool update_node_by_name(Cursor* cursor, Node* old_node, Node* new_node);

bool update_all_relationships(Cursor* cursor, Relationship* old_relationship, Relationship* new_relationship);

bool update_relationship_by_id(Cursor* cursor, Relationship* old_relationship, Relationship* new_relationship);

bool update_relationship_by_child(Cursor* cursor, Relationship* old_relationship, Relationship* new_relationship);

bool update_relationship_by_parent(Cursor* cursor, Relationship* old_relationship, Relationship* new_relationship);

bool update_all_properties(Cursor* cursor, Property* old_property, Property* new_property);

bool update_property_by_id(Cursor* cursor, Property* old_property, Property* new_property);

bool update_property_by_subject(Cursor* cursor, Property* old_property, Property* new_property);

bool update_property_by_key(Cursor* cursor, Property* old_property, Property* new_property);

Iterator* select_all_nodes(Cursor* cursor, Node* node);

Iterator* select_node_by_id(Cursor* cursor, Node* node);

Iterator* select_node_by_name(Cursor* cursor, Node* node);

Iterator* select_all_relationships(Cursor* cursor, Relationship* relationship);

Iterator* select_relationship_by_id(Cursor* cursor, Relationship* relationship);

Iterator* select_relationship_by_parent(Cursor* cursor, Relationship* relationship);

Iterator* select_relationship_by_child(Cursor* cursor, Relationship* relationship);

Iterator* select_all_properties(Cursor* cursor, Property* property);

Iterator* select_property_by_subject(Cursor* cursor, Property* property);

Iterator* select_property_by_key(Cursor* cursor, Property* property);

EntityIterator* select_properties_by_node(Cursor* cursor, Node* node);

EntityIterator* select_relationships_by_node(Cursor* cursor, Node* node);

EntityIterator* get_entity_iter_by_node(Cursor* cursor, Node* node, TypeOfElement element_type);

#endif

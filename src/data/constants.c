#include "../../include/data/constants.h"
#include "../../include/data/data.h"
#include "../../include/data/util_data.h"

const uint32_t PAGE_SIZE = 80;
const uint32_t ZERO = 0;
const uint64_t UINT32_T_SIZE = sizeof(uint32_t);
const uint64_t CHAR_SIZE = sizeof(char);
const uint64_t NODE_SIZE = sizeof(Node);
const uint64_t PROPERTY_SIZE = sizeof(Property);
const uint64_t RELATIONSHIP_SIZE = sizeof(Relationship);
const uint64_t ENTITY_SIZE = sizeof(Entity);
const uint64_t PAGE_HEADER_SIZE = sizeof(PageHeader);
const uint64_t VALUE_TYPE_SIZE = sizeof(ValueType);
const uint64_t TYPE_OF_ELEMENT_SIZE = sizeof(TypeOfElement);
const uint64_t PAGE_BODY_SIZE = (PAGE_SIZE - PAGE_HEADER_SIZE);
const uint32_t NAME_TYPE_WITH_TERM_LENGTH = (NAME_TYPE_LENGTH + 1);
const uint32_t NAME_TYPE_SIZE = NAME_TYPE_WITH_TERM_LENGTH * CHAR_SIZE;

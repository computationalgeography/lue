#include "lue/framework/api/c/local_operation.h"
#include "lue/framework/api/cxx/local_operation.hpp"


auto add(Field const* field1, Field const* field2) -> Field*
{
    lue::Field const& instance1 = *static_cast<lue::Field*>(field1->instance);
    lue::Field const& instance2 = *static_cast<lue::Field*>(field2->instance);
    lue::Field result = lue::add(instance1, instance2);

    return new Field{.instance = new lue::Field{std::move(result)}};
}

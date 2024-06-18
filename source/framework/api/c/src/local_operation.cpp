#include "lue/framework/api/c/local_operation.h"
#include "lue/framework/api/cxx/local_operation.hpp"


auto add(Field const* field1, Field const* field2) -> Field*
{
    lue::api::Field const& instance1 = *static_cast<lue::api::Field*>(field1->instance);
    lue::api::Field const& instance2 = *static_cast<lue::api::Field*>(field2->instance);
    lue::api::Field result = lue::api::add(instance1, instance2);

    return new Field{.instance = new lue::api::Field{std::move(result)}};
}

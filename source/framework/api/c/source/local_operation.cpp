#include "lue/framework/api/c/local_operation.h"
#include "field.hpp"
#include "lue/framework/api/cxx/local_operation.hpp"


auto add(Field const* field1, Field const* field2) -> Field*
{
    lue::api::Field result = lue::api::add(as_cxx_field(field1), as_cxx_field(field2));

    return new Field{.instance = new lue::api::Field{std::move(result)}};
}

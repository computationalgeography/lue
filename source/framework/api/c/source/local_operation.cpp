#include "lue/framework/api/c/local_operation.h"
#include "field.hpp"
#include "lue/framework/api/cxx/local_operation.hpp"


auto add(Field const* field1, Field const* field2) -> Field*
{
    lue::api::Field result = lue::api::add(as_cxx_field(field1), as_cxx_field(field2));

    return new Field{.instance = new lue::api::Field{std::move(result)}};
}


auto close_to(Field const* field1, Field const* field2) -> Field*
{
    lue::api::Field result = lue::api::close_to(as_cxx_field(field1), as_cxx_field(field2));

    return new Field{.instance = new lue::api::Field{std::move(result)}};
}


auto equal_to(Field const* field1, Field const* field2) -> Field*
{
    lue::api::Field result = lue::api::equal_to(as_cxx_field(field1), as_cxx_field(field2));

    return new Field{.instance = new lue::api::Field{std::move(result)}};
}


auto greater_than(Field const* field1, Field const* field2) -> Field*
{
    lue::api::Field result = lue::api::greater_than(as_cxx_field(field1), as_cxx_field(field2));

    return new Field{.instance = new lue::api::Field{std::move(result)}};
}


auto greater_than_equal_to(Field const* field1, Field const* field2) -> Field*
{
    lue::api::Field result = lue::api::greater_than_equal_to(as_cxx_field(field1), as_cxx_field(field2));

    return new Field{.instance = new lue::api::Field{std::move(result)}};
}


auto less_than(Field const* field1, Field const* field2) -> Field*
{
    lue::api::Field result = lue::api::less_than(as_cxx_field(field1), as_cxx_field(field2));

    return new Field{.instance = new lue::api::Field{std::move(result)}};
}


auto less_than_equal_to(Field const* field1, Field const* field2) -> Field*
{
    lue::api::Field result = lue::api::less_than_equal_to(as_cxx_field(field1), as_cxx_field(field2));

    return new Field{.instance = new lue::api::Field{std::move(result)}};
}


auto logical_and(Field const* field1, Field const* field2) -> Field*
{
    lue::api::Field result = lue::api::logical_and(as_cxx_field(field1), as_cxx_field(field2));

    return new Field{.instance = new lue::api::Field{std::move(result)}};
}


auto logical_exclusive_or(Field const* field1, Field const* field2) -> Field*
{
    lue::api::Field result = lue::api::logical_exclusive_or(as_cxx_field(field1), as_cxx_field(field2));

    return new Field{.instance = new lue::api::Field{std::move(result)}};
}


auto logical_inclusive_or(Field const* field1, Field const* field2) -> Field*
{
    lue::api::Field result = lue::api::logical_inclusive_or(as_cxx_field(field1), as_cxx_field(field2));

    return new Field{.instance = new lue::api::Field{std::move(result)}};
}


auto logical_not(Field const* field) -> Field*
{
    lue::api::Field result = lue::api::logical_not(as_cxx_field(field));

    return new Field{.instance = new lue::api::Field{std::move(result)}};
}


auto not_equal_to(Field const* field1, Field const* field2) -> Field*
{
    lue::api::Field result = lue::api::not_equal_to(as_cxx_field(field1), as_cxx_field(field2));

    return new Field{.instance = new lue::api::Field{std::move(result)}};
}

#include "lue/framework/api/c/local_operation.h"
#include "field.hpp"
#include "lue/framework/api/cxx/local_operation.hpp"


auto abs_(Field const* field) -> Field*
{
    lue::api::Field result = lue::api::abs(as_cxx_field(field));

    return new Field{.instance = new lue::api::Field{std::move(result)}};
}


auto acos_(Field const* field) -> Field*
{
    lue::api::Field result = lue::api::acos(as_cxx_field(field));

    return new Field{.instance = new lue::api::Field{std::move(result)}};
}


auto add(Field const* field1, Field const* field2) -> Field*
{
    lue::api::Field result = lue::api::add(as_cxx_field(field1), as_cxx_field(field2));

    return new Field{.instance = new lue::api::Field{std::move(result)}};
}


auto asin_(Field const* field) -> Field*
{
    lue::api::Field result = lue::api::asin(as_cxx_field(field));

    return new Field{.instance = new lue::api::Field{std::move(result)}};
}


auto atan_(Field const* field) -> Field*
{
    lue::api::Field result = lue::api::atan(as_cxx_field(field));

    return new Field{.instance = new lue::api::Field{std::move(result)}};
}


auto atan2_(Field const* field1, Field const* field2) -> Field*
{
    lue::api::Field result = lue::api::atan2(as_cxx_field(field1), as_cxx_field(field2));

    return new Field{.instance = new lue::api::Field{std::move(result)}};
}


auto ceil_(Field const* field) -> Field*
{
    lue::api::Field result = lue::api::ceil(as_cxx_field(field));

    return new Field{.instance = new lue::api::Field{std::move(result)}};
}


auto close_to(Field const* field1, Field const* field2) -> Field*
{
    lue::api::Field result = lue::api::close_to(as_cxx_field(field1), as_cxx_field(field2));

    return new Field{.instance = new lue::api::Field{std::move(result)}};
}


auto cos_(Field const* field) -> Field*
{
    lue::api::Field result = lue::api::cos(as_cxx_field(field));

    return new Field{.instance = new lue::api::Field{std::move(result)}};
}


auto divide(Field const* field1, Field const* field2) -> Field*
{
    lue::api::Field result = lue::api::divide(as_cxx_field(field1), as_cxx_field(field2));

    return new Field{.instance = new lue::api::Field{std::move(result)}};
}


auto equal_to(Field const* field1, Field const* field2) -> Field*
{
    lue::api::Field result = lue::api::equal_to(as_cxx_field(field1), as_cxx_field(field2));

    return new Field{.instance = new lue::api::Field{std::move(result)}};
}


auto exp_(Field const* field) -> Field*
{
    lue::api::Field result = lue::api::exp(as_cxx_field(field));

    return new Field{.instance = new lue::api::Field{std::move(result)}};
}


auto floor_(Field const* field) -> Field*
{
    lue::api::Field result = lue::api::floor(as_cxx_field(field));

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


auto log_(Field const* field) -> Field*
{
    lue::api::Field result = lue::api::log(as_cxx_field(field));

    return new Field{.instance = new lue::api::Field{std::move(result)}};
}


auto log10_(Field const* field) -> Field*
{
    lue::api::Field result = lue::api::log10(as_cxx_field(field));

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


auto modulus(Field const* field1, Field const* field2) -> Field*
{
    lue::api::Field result = lue::api::modulus(as_cxx_field(field1), as_cxx_field(field2));

    return new Field{.instance = new lue::api::Field{std::move(result)}};
}


auto multiply(Field const* field1, Field const* field2) -> Field*
{
    lue::api::Field result = lue::api::multiply(as_cxx_field(field1), as_cxx_field(field2));

    return new Field{.instance = new lue::api::Field{std::move(result)}};
}


auto negate(Field const* field) -> Field*
{
    lue::api::Field result = lue::api::negate(as_cxx_field(field));

    return new Field{.instance = new lue::api::Field{std::move(result)}};
}


auto not_equal_to(Field const* field1, Field const* field2) -> Field*
{
    lue::api::Field result = lue::api::not_equal_to(as_cxx_field(field1), as_cxx_field(field2));

    return new Field{.instance = new lue::api::Field{std::move(result)}};
}


auto open_simplex_noise(Field const* x_coordinates, Field const* y_coordinates, int const seed) -> Field*
{
    lue::api::Field result =
        lue::api::open_simplex_noise(as_cxx_field(x_coordinates), as_cxx_field(y_coordinates), seed);

    return new Field{.instance = new lue::api::Field{std::move(result)}};
}


auto pow(Field const* field1, Field const* field2) -> Field*
{
    lue::api::Field result = lue::api::pow(as_cxx_field(field1), as_cxx_field(field2));

    return new Field{.instance = new lue::api::Field{std::move(result)}};
}


auto round_(Field const* field) -> Field*
{
    lue::api::Field result = lue::api::round(as_cxx_field(field));

    return new Field{.instance = new lue::api::Field{std::move(result)}};
}


auto sin_(Field const* field) -> Field*
{
    lue::api::Field result = lue::api::sin(as_cxx_field(field));

    return new Field{.instance = new lue::api::Field{std::move(result)}};
}


auto sqrt_(Field const* field) -> Field*
{
    lue::api::Field result = lue::api::sqrt(as_cxx_field(field));

    return new Field{.instance = new lue::api::Field{std::move(result)}};
}


auto subtract(Field const* field1, Field const* field2) -> Field*
{
    lue::api::Field result = lue::api::subtract(as_cxx_field(field1), as_cxx_field(field2));

    return new Field{.instance = new lue::api::Field{std::move(result)}};
}


auto tan_(Field const* field) -> Field*
{
    lue::api::Field result = lue::api::tan(as_cxx_field(field));

    return new Field{.instance = new lue::api::Field{std::move(result)}};
}


auto trunc_(Field const* field) -> Field*
{
    lue::api::Field result = lue::api::trunc(as_cxx_field(field));

    return new Field{.instance = new lue::api::Field{std::move(result)}};
}


auto where(Field const* field1, Field const* field2, Field const* field3) -> Field*
{
    lue::api::Field result =
        field3 ? lue::api::where(as_cxx_field(field1), as_cxx_field(field2), as_cxx_field(field3))
               : lue::api::where(as_cxx_field(field1), as_cxx_field(field2));

    return new Field{.instance = new lue::api::Field{std::move(result)}};
}

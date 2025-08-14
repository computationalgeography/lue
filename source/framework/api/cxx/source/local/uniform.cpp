#include "lue/framework/api/cxx/local/uniform.hpp"
#include "lue/framework/algorithm/value_policies/uniform.hpp"
#include "lue/framework/api/cxx/detail/unsupported_overload.hpp"
#include "lue/overload.hpp"


namespace lue {
    namespace value_policies {

        auto uniform(auto const&... args) -> api::Field
        {
            api::detail::unsupported_overload("uniform", args...);

            return {};
        }

    }  // namespace value_policies


    namespace api {

        auto uniform(
            Shape<Count, 2> const& array_shape,
            Shape<Count, 2> const& partition_shape,
            Field const& min_value,
            Field const& max_value) -> Field
        {
            return std::visit(
                overload{
                    [&array_shape, &partition_shape](auto const& min_value, auto const& max_value) -> Field
                    { return value_policies::uniform(array_shape, partition_shape, min_value, max_value); }},
                min_value.variant(),
                max_value.variant());
        }


        auto uniform(Shape<Count, 2> const& array_shape, Field const& min_value, Field const& max_value)
            -> Field
        {
            return std::visit(
                overload{[&array_shape](auto const& min_value, auto const& max_value) -> Field {
                    return value_policies::uniform(array_shape, min_value, max_value);
                }},
                min_value.variant(),
                max_value.variant());
        }


        auto uniform(Field const& other, Field const& min_value, Field const& max_value) -> Field
        {
            return std::visit(
                overload{[](auto const& other, auto const& min_value, auto const& max_value) -> Field {
                    return value_policies::uniform(other, min_value, max_value);
                }},
                other.variant(),
                min_value.variant(),
                max_value.variant());
        }


        auto uniform(Field const& min_value, Field const& max_value) -> Field
        {
            return std::visit(
                overload{[](auto const& min_value, auto const& max_value) -> Field {
                    return value_policies::uniform(min_value, max_value);
                }},
                min_value.variant(),
                max_value.variant());
        }

    }  // namespace api
}  // namespace lue

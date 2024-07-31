#include "lue/framework/api/cxx/local/uniform.hpp"
#include "detail/unsupported_overload.hpp"
#include "overload.hpp"
#include "lue/framework/algorithm/value_policies/uniform.hpp"


namespace lue {
    namespace value_policies {

        auto uniform(
            [[maybe_unused]] auto const& array_shape,
            [[maybe_unused]] auto const& partition_shape,
            auto const& min_value,
            auto const& max_value) -> api::Field
        {
            api::detail::unsupported_overload("uniform", array_shape, partition_shape, min_value, max_value);

            return {};
        }


        auto uniform([[maybe_unused]] auto const& array_shape, auto const& min_value, auto const& max_value)
            -> api::Field
        {
            api::detail::unsupported_overload("uniform", array_shape, min_value, max_value);

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

    }  // namespace api
}  // namespace lue

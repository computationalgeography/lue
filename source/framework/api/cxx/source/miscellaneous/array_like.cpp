#include "lue/framework/api/cxx/miscellaneous/array_like.hpp"
#include "lue/framework/api/cxx/detail/unsupported_overload.hpp"
#include "lue/overload.hpp"
#include "lue/framework/algorithm/value_policies/array_like.hpp"


namespace lue {

    namespace value_policies {

        auto array_like([[maybe_unused]] auto const& array, auto const& fill_value) -> api::Array
        {
            api::detail::unsupported_overload("array_like", array, fill_value);

            return {};
        }

    }  // namespace value_policies


    namespace api {

        auto array_like(Array const& array, Scalar const& fill_value) -> Array
        {
            return std::visit(
                overload{
                    [](auto const& array, auto const& fill_value) -> Array
                    { return value_policies::array_like(array, fill_value); }},
                array.variant(),
                fill_value.variant());
        }

    }  // namespace api
}  // namespace lue

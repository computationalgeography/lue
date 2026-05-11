#include "lue/framework/api/cxx/local/floor.hpp"
#include "detail/unsupported_overload.hpp"
#include "overload.hpp"
#include "lue/framework/algorithm/value_policies/floor.hpp"


namespace lue {
    namespace value_policies {

        auto floor(auto const& field) -> api::Field
        {
            api::detail::unsupported_overload("floor", field);

            return {};
        }

    }  // namespace value_policies


    namespace api {

        auto floor(Field const& field) -> Field
        {
            return std::visit(
                overload{[](auto const& field) -> Field { return value_policies::floor(field); }},
                field.variant());
        }

    }  // namespace api
}  // namespace lue

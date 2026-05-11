#include "lue/framework/api/cxx/local/negate.hpp"
#include "detail/unsupported_overload.hpp"
#include "overload.hpp"
#include "lue/framework/algorithm/value_policies/negate.hpp"


namespace lue {
    namespace value_policies {

        auto negate(auto const& field) -> api::Field
        {
            api::detail::unsupported_overload("negate", field);

            return {};
        }

    }  // namespace value_policies


    namespace api {

        auto negate(Field const& field) -> Field
        {
            return std::visit(
                overload{[](auto const& field) -> Field { return value_policies::negate(field); }},
                field.variant());
        }

    }  // namespace api
}  // namespace lue

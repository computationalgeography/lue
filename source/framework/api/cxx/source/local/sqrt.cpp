#include "lue/framework/api/cxx/local/sqrt.hpp"
#include "detail/unsupported_overload.hpp"
#include "overload.hpp"
#include "lue/framework/algorithm/value_policies/sqrt.hpp"


namespace lue {
    namespace value_policies {

        auto sqrt(auto const& field) -> api::Field
        {
            api::detail::unsupported_overload("sqrt", field);

            return {};
        }

    }  // namespace value_policies


    namespace api {

        auto sqrt(Field const& field) -> Field
        {
            return std::visit(
                overload{[](auto const& field) -> Field { return value_policies::sqrt(field); }},
                field.variant());
        }

    }  // namespace api
}  // namespace lue

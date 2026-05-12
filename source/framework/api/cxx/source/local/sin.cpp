#include "lue/framework/api/cxx/local/sin.hpp"
#include "detail/unsupported_overload.hpp"
#include "overload.hpp"
#include "lue/framework/algorithm/value_policies/sin.hpp"


namespace lue {
    namespace value_policies {

        auto sin(auto const& field) -> api::Field
        {
            api::detail::unsupported_overload("sin", field);

            return {};
        }

    }  // namespace value_policies


    namespace api {

        auto sin(Field const& field) -> Field
        {
            return std::visit(
                overload{[](auto const& field) -> Field { return value_policies::sin(field); }},
                field.variant());
        }

    }  // namespace api
}  // namespace lue

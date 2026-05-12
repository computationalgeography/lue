#include "lue/framework/api/cxx/local/exp.hpp"
#include "detail/unsupported_overload.hpp"
#include "overload.hpp"
#include "lue/framework/algorithm/value_policies/exp.hpp"


namespace lue {
    namespace value_policies {

        auto exp(auto const& field) -> api::Field
        {
            api::detail::unsupported_overload("exp", field);

            return {};
        }

    }  // namespace value_policies


    namespace api {

        auto exp(Field const& field) -> Field
        {
            return std::visit(
                overload{[](auto const& field) -> Field { return value_policies::exp(field); }},
                field.variant());
        }

    }  // namespace api
}  // namespace lue

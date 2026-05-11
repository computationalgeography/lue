#include "lue/framework/api/cxx/local/trunc.hpp"
#include "detail/unsupported_overload.hpp"
#include "overload.hpp"
#include "lue/framework/algorithm/value_policies/trunc.hpp"


namespace lue {
    namespace value_policies {

        auto trunc(auto const& field) -> api::Field
        {
            api::detail::unsupported_overload("trunc", field);

            return {};
        }

    }  // namespace value_policies


    namespace api {

        auto trunc(Field const& field) -> Field
        {
            return std::visit(
                overload{[](auto const& field) -> Field { return value_policies::trunc(field); }},
                field.variant());
        }

    }  // namespace api
}  // namespace lue

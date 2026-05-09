#include "lue/framework/api/cxx/local/tan.hpp"
#include "detail/unsupported_overload.hpp"
#include "overload.hpp"
#include "lue/framework/algorithm/value_policies/tan.hpp"


namespace lue {
    namespace value_policies {

        auto tan(auto const& field) -> api::Field
        {
            api::detail::unsupported_overload("tan", field);

            return {};
        }

    }  // namespace value_policies


    namespace api {

        auto tan(Field const& field) -> Field
        {
            return std::visit(
                overload{[](auto const& field) -> Field { return value_policies::tan(field); }},
                field.variant());
        }

    }  // namespace api
}  // namespace lue

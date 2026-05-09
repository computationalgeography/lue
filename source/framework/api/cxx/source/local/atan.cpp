#include "lue/framework/api/cxx/local/atan.hpp"
#include "detail/unsupported_overload.hpp"
#include "overload.hpp"
#include "lue/framework/algorithm/value_policies/atan.hpp"


namespace lue {
    namespace value_policies {

        auto atan(auto const& field) -> api::Field
        {
            api::detail::unsupported_overload("atan", field);

            return {};
        }

    }  // namespace value_policies


    namespace api {

        auto atan(Field const& field) -> Field
        {
            return std::visit(
                overload{[](auto const& field) -> Field { return value_policies::atan(field); }},
                field.variant());
        }

    }  // namespace api
}  // namespace lue

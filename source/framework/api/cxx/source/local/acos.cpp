#include "lue/framework/api/cxx/local/acos.hpp"
#include "detail/unsupported_overload.hpp"
#include "overload.hpp"
#include "lue/framework/algorithm/value_policies/acos.hpp"


namespace lue {
    namespace value_policies {

        auto acos(auto const& field) -> api::Field
        {
            api::detail::unsupported_overload("acos", field);

            return {};
        }

    }  // namespace value_policies


    namespace api {

        auto acos(Field const& field) -> Field
        {
            return std::visit(
                overload{[](auto const& field) -> Field { return value_policies::acos(field); }},
                field.variant());
        }

    }  // namespace api
}  // namespace lue

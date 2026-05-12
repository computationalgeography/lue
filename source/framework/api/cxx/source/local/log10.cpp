#include "lue/framework/api/cxx/local/log10.hpp"
#include "detail/unsupported_overload.hpp"
#include "overload.hpp"
#include "lue/framework/algorithm/value_policies/log10.hpp"


namespace lue {
    namespace value_policies {

        auto log10(auto const& field) -> api::Field
        {
            api::detail::unsupported_overload("log10", field);

            return {};
        }

    }  // namespace value_policies


    namespace api {

        auto log10(Field const& field) -> Field
        {
            return std::visit(
                overload{[](auto const& field) -> Field { return value_policies::log10(field); }},
                field.variant());
        }

    }  // namespace api
}  // namespace lue

#include "lue/framework/api/cxx/local/ceil.hpp"
#include "detail/unsupported_overload.hpp"
#include "overload.hpp"
#include "lue/framework/algorithm/value_policies/ceil.hpp"


namespace lue {
    namespace value_policies {

        auto ceil(auto const& field) -> api::Field
        {
            api::detail::unsupported_overload("ceil", field);

            return {};
        }

    }  // namespace value_policies


    namespace api {

        auto ceil(Field const& field) -> Field
        {
            return std::visit(
                overload{[](auto const& field) -> Field { return value_policies::ceil(field); }},
                field.variant());
        }

    }  // namespace api
}  // namespace lue

#include "lue/framework/api/cxx/local/asin.hpp"
#include "detail/unsupported_overload.hpp"
#include "overload.hpp"
#include "lue/framework/algorithm/value_policies/asin.hpp"


namespace lue {
    namespace value_policies {

        auto asin(auto const& field) -> api::Field
        {
            api::detail::unsupported_overload("asin", field);

            return {};
        }

    }  // namespace value_policies


    namespace api {

        auto asin(Field const& field) -> Field
        {
            return std::visit(
                overload{[](auto const& field) -> Field { return value_policies::asin(field); }},
                field.variant());
        }

    }  // namespace api
}  // namespace lue

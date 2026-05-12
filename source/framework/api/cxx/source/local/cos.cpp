#include "lue/framework/api/cxx/local/cos.hpp"
#include "detail/unsupported_overload.hpp"
#include "overload.hpp"
#include "lue/framework/algorithm/value_policies/cos.hpp"


namespace lue {
    namespace value_policies {

        auto cos(auto const& field) -> api::Field
        {
            api::detail::unsupported_overload("cos", field);

            return {};
        }

    }  // namespace value_policies


    namespace api {

        auto cos(Field const& field) -> Field
        {
            return std::visit(
                overload{[](auto const& field) -> Field { return value_policies::cos(field); }},
                field.variant());
        }

    }  // namespace api
}  // namespace lue

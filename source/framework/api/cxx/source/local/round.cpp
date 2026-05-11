#include "lue/framework/api/cxx/local/round.hpp"
#include "detail/unsupported_overload.hpp"
#include "overload.hpp"
#include "lue/framework/algorithm/value_policies/round.hpp"


namespace lue {
    namespace value_policies {

        auto round(auto const& field) -> api::Field
        {
            api::detail::unsupported_overload("round", field);

            return {};
        }

    }  // namespace value_policies


    namespace api {

        auto round(Field const& field) -> Field
        {
            return std::visit(
                overload{[](auto const& field) -> Field { return value_policies::round(field); }},
                field.variant());
        }

    }  // namespace api
}  // namespace lue

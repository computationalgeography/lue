#include "lue/framework/api/cxx/local/log.hpp"
#include "detail/unsupported_overload.hpp"
#include "overload.hpp"
#include "lue/framework/algorithm/value_policies/log.hpp"


namespace lue {
    namespace value_policies {

        auto log(auto const& field) -> api::Field
        {
            api::detail::unsupported_overload("log", field);

            return {};
        }

    }  // namespace value_policies


    namespace api {

        auto log(Field const& field) -> Field
        {
            return std::visit(
                overload{[](auto const& field) -> Field { return value_policies::log(field); }},
                field.variant());
        }

    }  // namespace api
}  // namespace lue

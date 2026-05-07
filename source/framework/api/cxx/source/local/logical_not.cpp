#include "lue/framework/api/cxx/local/logical_not.hpp"
#include "detail/unsupported_overload.hpp"
#include "overload.hpp"
#include "lue/framework/algorithm/value_policies/logical_not.hpp"
#include "lue/framework.hpp"


namespace lue {
    namespace value_policies {

        template<typename OutputElement>
        auto logical_not(auto const& field) -> api::Field
        {
            api::detail::unsupported_overload("logical_not", field);

            return {};
        }

    }  // namespace value_policies


    namespace api {

        auto logical_not(Field const& field) -> Field
        {
            return std::visit(
                overload{
                    [](auto const& field) -> Field
                    { return value_policies::logical_not<BooleanElement>(field); }},
                field.variant());
        }

    }  // namespace api
}  // namespace lue

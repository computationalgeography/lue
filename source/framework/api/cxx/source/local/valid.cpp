#include "lue/framework/api/cxx/local/valid.hpp"
#include "detail/unsupported_overload.hpp"
#include "overload.hpp"
#include "lue/framework/algorithm/value_policies/valid.hpp"
#include "lue/framework.hpp"


namespace lue {
    namespace value_policies {

        template<typename OutputElement>
        auto valid(auto const& field) -> api::Field
        {
            api::detail::unsupported_overload("valid", field);

            return {};
        }

    }  // namespace value_policies


    namespace api {

        auto valid(Field const& field) -> Field
        {
            return std::visit(
                overload{
                    [](auto const& field) -> Field { return value_policies::valid<BooleanElement>(field); }},
                field.variant());
        }

    }  // namespace api
}  // namespace lue

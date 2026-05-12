#include "detail/unsupported_overload.hpp"
#include "overload.hpp"
#include "lue/framework/algorithm/value_policies/where.hpp"
#include "lue/framework/api/cxx/local/where.hpp"
#include "lue/framework.hpp"


namespace lue {
    namespace value_policies {

        template<typename OutputElement>
        auto where(auto const& field1, auto const& field2, auto const& field3) -> api::Field
        {
            api::detail::unsupported_overload("where", field1, field2, field3);

            return {};
        }

    }  // namespace value_policies


    namespace api {

        auto where(Field const& field1, Field const& field2, Field const& field3) -> Field
        {
            return std::visit(
                overload{
                    [](auto const& field1, auto const& field2, auto const& field3) -> Field
                    { return value_policies::where<BooleanElement>(field1, field2, field3); }},
                field1.variant(),
                field2.variant(),
                field3.variant());
        }

    }  // namespace api
}  // namespace lue

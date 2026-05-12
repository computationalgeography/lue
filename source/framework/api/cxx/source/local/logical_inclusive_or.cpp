#include "lue/framework/api/cxx/local/logical_inclusive_or.hpp"
#include "detail/unsupported_overload.hpp"
#include "overload.hpp"
#include "lue/framework/algorithm/value_policies/logical_inclusive_or.hpp"
#include "lue/framework.hpp"


namespace lue {
    namespace value_policies {

        template<typename OutputElement>
        auto logical_inclusive_or(auto const& field1, auto const& field2) -> api::Field
        {
            api::detail::unsupported_overload("logical_inclusive_or", field1, field2);

            return {};
        }

    }  // namespace value_policies


    namespace api {

        auto logical_inclusive_or(Field const& field1, Field const& field2) -> Field
        {
            return std::visit(
                overload{
                    [](auto const& field1, auto const& field2) -> Field
                    { return value_policies::logical_inclusive_or<BooleanElement>(field1, field2); }},
                field1.variant(),
                field2.variant());
        }

    }  // namespace api
}  // namespace lue

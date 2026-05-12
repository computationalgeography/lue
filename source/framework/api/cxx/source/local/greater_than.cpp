#include "lue/framework/api/cxx/local/greater_than.hpp"
#include "detail/unsupported_overload.hpp"
#include "overload.hpp"
#include "lue/framework/algorithm/value_policies/greater_than.hpp"


namespace lue {
    namespace value_policies {

        template<typename OutputElement>
        auto greater_than(auto const& field1, auto const& field2) -> api::Field
        {
            api::detail::unsupported_overload("greater_than", field1, field2);

            return {};
        }

    }  // namespace value_policies


    namespace api {

        auto greater_than(Field const& field1, Field const& field2) -> Field
        {
            return std::visit(
                overload{
                    [](auto const& field1, auto const& field2) -> Field
                    { return value_policies::greater_than<BooleanElement>(field1, field2); }},
                field1.variant(),
                field2.variant());
        }

    }  // namespace api
}  // namespace lue

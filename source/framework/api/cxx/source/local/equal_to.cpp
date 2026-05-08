#include "lue/framework/api/cxx/local/equal_to.hpp"
#include "detail/unsupported_overload.hpp"
#include "overload.hpp"
#include "lue/framework/algorithm/value_policies/equal_to.hpp"


namespace lue {
    namespace value_policies {

        template<typename OutputElement>
        auto equal_to(auto const& field1, auto const& field2) -> api::Field
        {
            api::detail::unsupported_overload("equal_to", field1, field2);

            return {};
        }

    }  // namespace value_policies


    namespace api {

        auto equal_to(Field const& field1, Field const& field2) -> Field
        {
            return std::visit(
                overload{
                    [](auto const& field1, auto const& field2) -> Field
                    { return value_policies::equal_to<BooleanElement>(field1, field2); }},
                field1.variant(),
                field2.variant());
        }

    }  // namespace api
}  // namespace lue

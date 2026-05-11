#include "lue/framework/api/cxx/local/multiply.hpp"
#include "detail/unsupported_overload.hpp"
#include "overload.hpp"
#include "lue/framework/algorithm/value_policies/multiply.hpp"


namespace lue {
    namespace value_policies {

        auto multiply(auto const& field1, auto const& field2) -> api::Field
        {
            api::detail::unsupported_overload("multiply", field1, field2);

            return {};
        }

    }  // namespace value_policies


    namespace api {

        auto multiply(Field const& field1, Field const& field2) -> Field
        {
            return std::visit(
                overload{
                    [](auto const& field1, auto const& field2) -> Field
                    { return value_policies::multiply(field1, field2); }},
                field1.variant(),
                field2.variant());
        }

    }  // namespace api
}  // namespace lue

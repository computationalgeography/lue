#include "lue/framework/api/cxx/local/divide.hpp"
#include "detail/unsupported_overload.hpp"
#include "overload.hpp"
#include "lue/framework/algorithm/value_policies/divide.hpp"


namespace lue {
    namespace value_policies {

        auto divide(auto const& field1, auto const& field2) -> api::Field
        {
            api::detail::unsupported_overload("divide", field1, field2);

            return {};
        }

    }  // namespace value_policies


    namespace api {

        auto divide(Field const& field1, Field const& field2) -> Field
        {
            return std::visit(
                overload{
                    [](auto const& field1, auto const& field2) -> Field
                    { return value_policies::divide(field1, field2); }},
                field1.variant(),
                field2.variant());
        }

    }  // namespace api
}  // namespace lue

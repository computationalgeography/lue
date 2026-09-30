#include "lue/framework/api/cxx/routing/d8_flow_direction.hpp"
#include "lue/framework/algorithm/value_policies/d8_flow_direction.hpp"
#include "lue/overload.hpp"
#include "lue/framework/api/cxx/detail/unsupported_overload.hpp"
#include "lue/framework/configure.hpp"


namespace lue {
    namespace value_policies {

        template<typename FlowDirectionElement>
        auto d8_flow_direction(auto const& elevation) -> api::Field
        {
            api::detail::unsupported_overload("d8_flow_direction", elevation);

            return {};
        }

    }  // namespace value_policies


    namespace api {

        auto d8_flow_direction(Field const& elevation) -> Field
        {
            return std::visit(
                overload{[](auto const& elevation) -> Field {
                    return value_policies::d8_flow_direction<FlowDirectionElement>(elevation);
                }},
                elevation.variant());
        }

    }  // namespace api
}  // namespace lue

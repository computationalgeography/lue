#include "lue/framework/api/cxx/routing/accu.hpp"
#include "detail/unsupported_overload.hpp"
#include "overload.hpp"
#include "lue/framework/algorithm/value_policies/accu.hpp"


namespace lue {
    namespace value_policies {

        auto accu(auto const& flow_direction, auto const& inflow) -> api::Field
        {
            api::detail::unsupported_overload("accu", flow_direction, inflow);

            return {};
        }

    }  // namespace value_policies


    namespace api {

        /*!
            @brief      TODO
            @param      .
            @return     .
            @exception  .
        */
        auto accu(FlowDirectionField const& flow_direction, Field const& inflow) -> Field
        {
            return std::visit(
                overload{
                    [](auto const& flow_direction, auto const& inflow) -> Field
                    { return value_policies::accu(flow_direction, inflow); }},
                flow_direction.variant(),
                inflow.variant());
        }

    }  // namespace api
}  // namespace lue

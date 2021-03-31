#include "lue/framework/algorithm/accu_fraction.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        Rank const rank{2};
        using FlowDirectionElement = std::uint8_t;

        template<
            typename FractionElement>
        std::tuple<PartitionedArray<FractionElement, rank>, PartitionedArray<FractionElement, rank>>
            accu_fraction(
                PartitionedArray<FlowDirectionElement, rank> const& flow_direction,
                PartitionedArray<FractionElement, rank> const& material,
                PartitionedArray<FractionElement, rank> const& fraction)
        {
            using Policies =
                policy::accu_fraction::DefaultValuePolicies<FlowDirectionElement, FractionElement>;

            return accu_fraction(Policies{}, flow_direction, material, fraction);
        }

    }  // Anonymous namespace


    void bind_accu_fraction(
        pybind11::module& module)
    {
        module.def("accu_fraction", accu_fraction<float>);
        module.def("accu_fraction", accu_fraction<double>);
    }

}  // namespace lue::framework

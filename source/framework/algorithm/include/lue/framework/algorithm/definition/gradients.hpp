#pragma once
#include "lue/framework/algorithm/definition/convolve.hpp"
#include "lue/framework/algorithm/definition/divide.hpp"
#include "lue/framework/algorithm/focal_operation_export.hpp"
#include "lue/framework/algorithm/gradients.hpp"
#include "lue/framework/algorithm/kernel.hpp"
#include "lue/framework/algorithm/serialize/kernel.hpp"


namespace lue {

    template<typename Policies, typename Element>
    Gradients<Element> gradients(
        [[maybe_unused]] Policies const& policies,
        PartitionedArray<Element, 2> const& elevation,
        Element const cell_size)
    {
        static_assert(std::is_floating_point_v<Element>);

        using Array = PartitionedArray<Element, 2>;

        using Kernel = lue::Kernel<Element, 2>;
        using KernelShape = ShapeT<Kernel>;

        KernelShape kernel_shape{3, 3};

        Kernel dz_dx_kernel{
            kernel_shape,
            {
                -1.0,
                0.0,
                1.0,
                -2.0,
                0.0,
                2.0,
                -1.0,
                0.0,
                1.0,
            }};

        Kernel dz_dy_kernel{
            kernel_shape,
            {
                -1.0,
                -2.0,
                -1.0,
                0.0,
                0.0,
                0.0,
                1.0,
                2.0,
                1.0,
            }};

        // TODO(KDJ) Make dependent on Policies
        // Currently, halo is filled with no-data...
        auto convolve_policies =
            policy::convolve::DefaultPolicies<Element, Element>{lue::policy::no_data_value<Element>};

        using DividePolicies = policy::Policies<
            policy::divide::DomainPolicy<Element>,
            policy::OutputsPolicies<policy::OutputPoliciesT<Policies, 0, Element>>,
            policy::InputsPolicies<
                policy::InputPoliciesT<Policies, 0, Element>,
                policy::InputPoliciesT<Policies, 0, Element>>>;

        DividePolicies divide_policies{};

        Array dz_dx = divide(
            divide_policies,
            convolve<Element>(convolve_policies, elevation, dz_dx_kernel),
            Element{8} * cell_size);
        Array dz_dy = divide(
            divide_policies,
            convolve<Element>(convolve_policies, elevation, dz_dy_kernel),
            Element{8} * cell_size);

        return {std::move(dz_dx), std::move(dz_dy)};
    }

}  // namespace lue


#define LUE_INSTANTIATE_GRADIENTS(Policies, Element)                                                         \
                                                                                                             \
    template LUE_FOCAL_OPERATION_EXPORT Gradients<Element> gradients<ArgumentType<void(Policies)>, Element>( \
        ArgumentType<void(Policies)> const&, PartitionedArray<Element, 2> const&, Element const);

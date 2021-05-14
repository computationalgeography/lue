#pragma once
#include "lue/framework/algorithm/slope.hpp"
#include "lue/framework/algorithm/definition/add.hpp"
#include "lue/framework/algorithm/definition/divide.hpp"
#include "lue/framework/algorithm/definition/convolve.hpp"
#include "lue/framework/algorithm/kernel.hpp"
#include "lue/framework/algorithm/serialize/kernel.hpp"
#include "lue/framework/algorithm/definition/pow.hpp"
#include "lue/framework/algorithm/sqrt.hpp"


namespace lue {

    /*!
        @brief      Return slope of elevation surface
        @param      elevation Elevation of the terrain
        @param      cell_size Cell size
        @return     Slope field with slope in percentage
        @warning    The result is not correct yet for the cells at the border
                    of the array and the handling of no-data cells (#gh193).

        The units of @a elevation and @a cell_size should be equal.

        To convert the result to degrees, use:

        @code
        slope_in_degrees = atan(slope_in_percentage)
        @endcode

        To convert to radians, use:

        @code
        slope_in_radians = slope_in_degrees * 180 / M_PI
        @endcode

        This algorithm implements Horne's slope algorithm (Horn, B.K.P. (1981)
        Hill shading and the reflectance map. Proceedings of IEEE 69(1), 14-47).
        In pseudo-code this works as folows:

        @code
        dz_dx = convolve(value, dz_dx_kernel) / (8 * cell_size)
        dz_dy = convolve(value, dz_dy_kernel) / (8 * cell_size)
        result = sqrt(pow(dz_dx, 2) + pow(dz_dy, 2))
        @endcode

        where dz_dx_kernel is:

        @code
        +----+----+----+
        |  1 |  0 | -1 |
        +----+----+----+
        |  2 |  0 | -2 |
        +----+----+----+
        |  1 |  0 | -1 |
        +----+----+----+
        @endcode

        and dz_dy_kernel is:

        @code
        +----+----+----+
        | -1 | -2 | -1 |
        +----+----+----+
        |  0 |  0 |  0 |
        +----+----+----+
        |  1 |  2 |  1 |
        +----+----+----+
        @endcode
    */
    template<
        typename Policies,
        typename Element>
    PartitionedArray<Element, 2> slope(
        [[maybe_unused]] Policies const& policies,  // Only the type is used atm
        PartitionedArray<Element, 2> const& elevation,
        Element const cell_size)
    {
        static_assert(std::is_floating_point_v<Element>);

        using Array = PartitionedArray<Element, 2>;

        using Kernel = lue::Kernel<Element, 2>;
        using KernelShape = ShapeT<Kernel>;

        KernelShape kernel_shape{3, 3};

        Kernel dz_dx_kernel{kernel_shape, {
                 1.0,  0.0, -1.0,
                 2.0,  0.0, -2.0,
                 1.0,  0.0, -1.0,
            }};

        Kernel dz_dy_kernel{kernel_shape, {
                -1.0, -2.0, -1.0,
                 0.0,  0.0,  0.0,
                 1.0,  2.0,  1.0,
            }};

        // TODO(KDJ) Make dependent on Policies
        // Currently, halo is filled with no-data...
        auto convolve_policies = policy::convolve::DefaultPolicies<Element, Element>{
            lue::policy::no_data_value<Element>};

        using DividePolicies = policy::Policies<
                policy::divide::DomainPolicy<Element>,
                policy::OutputsPolicies<
                        policy::OutputPoliciesT<Policies, 0, Element>
                    >,
                policy::InputsPolicies<
                        policy::InputPoliciesT<Policies, 0, Element>,
                        policy::InputPoliciesT<Policies, 0, Element>
                    >
            >;

        DividePolicies divide_policies{};

        Array dz_dx =
            divide(divide_policies,
                convolve<Element>(convolve_policies, elevation, dz_dx_kernel),
                Element{8} * cell_size);
        Array dz_dy =
            divide(divide_policies,
                convolve<Element>(convolve_policies, elevation, dz_dy_kernel),
                Element{8} * cell_size);

        using PowPolicies = policy::Policies<
                policy::pow::DomainPolicy<Element>,
                policy::OutputsPolicies<
                        policy::OutputPoliciesT<DividePolicies, 0, Element>
                    >,
                policy::InputsPolicies<
                        policy::InputPoliciesT<DividePolicies, 0, Element>,
                        policy::InputPolicies<policy::SkipNoData<Element>>
                    >
            >;
        using AddPolicies = policy::Policies<
                policy::add::DomainPolicy<Element>,
                policy::OutputsPolicies<
                        policy::OutputPoliciesT<PowPolicies, 0, Element>
                    >,
                policy::InputsPolicies<
                        policy::InputPoliciesT<PowPolicies, 0, Element>,
                        policy::InputPoliciesT<PowPolicies, 0, Element>
                    >
            >;
        using SqrtPolicies = policy::Policies<
                policy::sqrt::DomainPolicy<Element>,
                policy::OutputsPolicies<
                        policy::OutputPoliciesT<AddPolicies, 0, Element>
                    >,
                policy::InputsPolicies<
                        policy::InputPoliciesT<AddPolicies, 0, Element>
                    >
            >;

        // auto sqrt_policies = policy::sqrt::DefaultValuePolicies<Element>{};
        // auto add_policies = policy::add::DefaultValuePolicies<Element, Element>{};
        // auto pow_policies = policy::pow::DefaultValuePolicies<Element, Element>{};

        PowPolicies pow_policies{};
        AddPolicies add_policies{};
        SqrtPolicies sqrt_policies{};

        return sqrt(sqrt_policies,
                add(add_policies,
                            pow(pow_policies, dz_dx, Element{2}),
                            pow(pow_policies, dz_dy, Element{2})
                    )
            );
    }

}  // namespace lue


#define LUE_INSTANTIATE_SLOPE(Policies, Element)                                \
                                                                                \
    template LUE_FA_EXPORT                                                      \
    PartitionedArray<Element, 2> slope<ArgumentType<void(Policies)>, Element>(  \
        ArgumentType<void(Policies)> const&,                                    \
        PartitionedArray<Element, 2> const&,                                    \
        Element const);

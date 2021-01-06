#pragma once
#include "lue/framework/algorithm/arithmetic.hpp"
#include "lue/framework/algorithm/convolve.hpp"
#include "lue/framework/algorithm/kernel.hpp"
#include "lue/framework/algorithm/serialize/kernel.hpp"
#include "lue/framework/algorithm/pow.hpp"
#include "lue/framework/algorithm/sqrt.hpp"


namespace lue {
namespace policy {
namespace slope {

template<
    typename Element>
using DefaultPolicies = policy::DefaultPolicies<
    OutputElements<Element>,
    InputElements<Element, Element>>;

}  // namespace slope
}  // namespace policy


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
    Policies const& /* policies */,
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

    // Array dz_dx = convolve(elevation, dz_dx_kernel) / (8.0 * cell_size);
    // Array dz_dy = convolve(elevation, dz_dy_kernel) / (8.0 * cell_size);

    // using DividePolicies =
    //     Policies<
    //             InputNoDataPolicies<>,
    //             OutputNoDataPolicies<>
    //         >;

    // TODO(KDJ) Make dependent on Policies
    // auto convolve_policies = policy::convolve::DefaultPolicies{};

    // TODO(KDJ) Make dependent on Policies

    // using DividePolicies = policy::divide::Policies<
    //     InputNoDataPolicyT<Policies, 0>,
    //     SkipNoData,
    //     OutputNoDataPolyT<Policies, 0>>;

    auto divide_policies = policy::divide::DefaultPolicies<Element, Element>{};

    // This assumes 8 * cell_size does not go out of range, which would be silly
    Array dz_dx = divide(divide_policies, convolve(elevation, dz_dx_kernel), 8.0 * cell_size);
    Array dz_dy = divide(divide_policies, convolve(elevation, dz_dy_kernel), 8.0 * cell_size);

    // return sqrt(pow(dz_dx, 2.0) + pow(dz_dy, 2.0));

    auto sqrt_policies = policy::sqrt::DefaultPolicies<Element>{};
    auto add_policies = policy::add::DefaultPolicies<Element, Element>{};
    auto pow_policies = policy::pow::DefaultPolicies<Element, Element>{};

    return sqrt(sqrt_policies,
        add(add_policies, pow(pow_policies, dz_dx, 2.0), pow(pow_policies, dz_dy, 2.0)));
}


template<
    typename Element>
PartitionedArray<Element, 2> slope(
    PartitionedArray<Element, 2> const& elevation,
    Element const cell_size)
{
    using Policies = policy::slope::DefaultPolicies<Element>;

    return slope(Policies{}, elevation, cell_size);
}

}  // namespace lue

#pragma once
#include "lue/framework/algorithm/arithmetic.hpp"
#include "lue/framework/algorithm/convolve.hpp"
#include "lue/framework/algorithm/kernel.hpp"
#include "lue/framework/algorithm/serialize/kernel.hpp"
#include "lue/framework/algorithm/pow.hpp"
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
    typename Element>
PartitionedArray<Element, 2> slope(
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

    Array dz_dx = convolve(elevation, dz_dx_kernel) / (8.0 * cell_size);
    Array dz_dy = convolve(elevation, dz_dy_kernel) / (8.0 * cell_size);

    return sqrt(pow(dz_dx, 2.0) + pow(dz_dy, 2.0));
}

}  // namespace lue

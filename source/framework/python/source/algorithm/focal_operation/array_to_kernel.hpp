#pragma once
#include "lue/framework/algorithm/kernel.hpp"
#include <pybind11/numpy.h>


namespace lue::framework {

    template<typename Weight, Rank rank, typename Element>
    Kernel<Weight, rank> array_to_kernel(pybind11::array_t<Element> const& array)
    {
        using Kernel = lue::Kernel<Weight, rank>;
        using Shape = typename Kernel::Shape;

        // Verify rank of shape of array corresponds with the one requested
        if (array.ndim() != rank)
        {
            throw std::runtime_error(std::format(
                "Rank of array shape must be equal to rank of array ({} != {})", array.ndim(), 2));
        }

        Shape shape{};
        std::copy_n(array.shape(), rank, shape.begin());

        if (!is_hypercube(shape))
        {
            throw std::runtime_error(
                std::format("Shape of array to use as kernel must be a hypercube (a square in 2D)"));
        }


        // TODO In the current case (uint8 to uint8) all is well. In general it might not be.
        static_assert(std::is_same_v<Element, Weight>);
        // if(!is_convertible<Element, Weight>)
        // {
        // }

        pybind11::buffer_info buffer{array.request()};

        Kernel kernel{shape};
        std::copy_n(static_cast<Element*>(buffer.ptr), buffer.size, kernel.begin());

        return kernel;
    }

}  // namespace lue::framework

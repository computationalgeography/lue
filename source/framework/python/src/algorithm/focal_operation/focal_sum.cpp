#include "lue/framework/algorithm/value_policies/focal_sum.hpp"
#include "lue/framework/algorithm/kernel.hpp"
#include <pybind11/numpy.h>
// #include <pybind11/pybind11.h>


using namespace pybind11::literals;


namespace lue::framework {
    namespace {


        template<
            typename Weight,
            Rank rank,
            typename Element>
        Kernel<Weight, rank> array_to_kernel(
            pybind11::array_t<Element> const& array)
        {
            using Kernel = lue::Kernel<Weight, rank>;
            using Shape = typename Kernel::Shape;

            // Verify rank of shape of array corresponds with the one requested
            if(array.ndim() != rank)
            {
                throw std::runtime_error(fmt::format(
                    "Rank of array shape must be equal to rank of array ({} != {})",
                    array.ndim(), 2));
            }

            Shape shape{};
            std::copy_n(array.shape(), rank, shape.begin());

            if(!is_hypercube(shape))
            {
                throw std::runtime_error(fmt::format(
                    "Shape of array to use as kernel must be a hypercube (a square in 2D)"));
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


        template<
            typename Element,
            Rank rank>
        PartitionedArray<Element, rank> focal_sum(
            PartitionedArray<Element, rank> const& array,
            pybind11::array_t<std::uint8_t> const& kernel)
        {
            return value_policies::focal_sum(array, array_to_kernel<std::uint8_t, rank>(kernel));
        }

    }  // Anonymous namespace


    void bind_focal_sum(
        pybind11::module& module)
    {
        module.def("focal_sum", focal_sum<std::uint8_t, 2>, "array"_a, "kernel"_a.noconvert());
        module.def("focal_sum", focal_sum<std::int32_t, 2>, "array"_a, "kernel"_a.noconvert());
        module.def("focal_sum", focal_sum<std::uint32_t, 2>, "array"_a, "kernel"_a.noconvert());
        module.def("focal_sum", focal_sum<std::int64_t, 2>, "array"_a, "kernel"_a.noconvert());
        module.def("focal_sum", focal_sum<std::uint64_t, 2>, "array"_a, "kernel"_a.noconvert());
        module.def("focal_sum", focal_sum<float, 2>, "array"_a, "kernel"_a.noconvert());
        module.def("focal_sum", focal_sum<double, 2>, "array"_a, "kernel"_a.noconvert());
    }

}  // namespace lue::framework

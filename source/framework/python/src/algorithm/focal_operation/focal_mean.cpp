#include "lue/framework/algorithm/value_policies/focal_mean.hpp"
#include "array_to_kernel.hpp"


using namespace pybind11::literals;


namespace lue::framework {
    namespace {

        template<
            typename Element,
            Rank rank>
        PartitionedArray<Element, rank> focal_mean(
            PartitionedArray<Element, rank> const& array,
            pybind11::array_t<std::uint8_t> const& kernel)
        {
            return value_policies::focal_mean(array, array_to_kernel<std::uint8_t, rank>(kernel));
        }

    }  // Anonymous namespace


    void bind_focal_mean(
        pybind11::module& module)
    {
        module.def("focal_mean", focal_mean<float, 2>, "array"_a, "kernel"_a.noconvert());
        module.def("focal_mean", focal_mean<double, 2>, "array"_a, "kernel"_a.noconvert());
    }

}  // namespace lue::framework

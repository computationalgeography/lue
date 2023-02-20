#include "lue/framework/algorithm/value_policies/focal_high_pass.hpp"
#include "array_to_kernel.hpp"


using namespace pybind11::literals;


namespace lue::framework {
    namespace {

        template<typename Element, Rank rank>
        PartitionedArray<Element, rank> focal_high_pass(
            PartitionedArray<Element, rank> const& array, pybind11::array_t<std::uint8_t> const& kernel)
        {
            return value_policies::focal_high_pass(array, array_to_kernel<std::uint8_t, rank>(kernel));
        }

    }  // Anonymous namespace


    void bind_focal_high_pass(pybind11::module& module)
    {
        module.def("focal_high_pass", focal_high_pass<float, 2>, "array"_a, "kernel"_a.noconvert());
        module.def("focal_high_pass", focal_high_pass<double, 2>, "array"_a, "kernel"_a.noconvert());
    }

}  // namespace lue::framework

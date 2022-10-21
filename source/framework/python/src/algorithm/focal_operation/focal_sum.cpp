#include "lue/framework/algorithm/value_policies/focal_sum.hpp"
#include "array_to_kernel.hpp"


using namespace pybind11::literals;


namespace lue::framework {
    namespace {

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

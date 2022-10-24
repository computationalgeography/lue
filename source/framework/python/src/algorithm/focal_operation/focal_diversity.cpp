#include "lue/framework/algorithm/value_policies/focal_diversity.hpp"
#include "array_to_kernel.hpp"


using namespace pybind11::literals;


namespace lue::framework {
    namespace {

        template<
            typename Count,
            typename Element,
            Rank rank>
        PartitionedArray<Count, rank> focal_diversity(
            PartitionedArray<Element, rank> const& array,
            pybind11::array_t<std::uint8_t> const& kernel)
        {
            return value_policies::focal_diversity<Count>(array, array_to_kernel<std::uint8_t, rank>(kernel));
        }

    }  // Anonymous namespace


    void bind_focal_diversity(
        pybind11::module& module)
    {
        module.def("focal_diversity", focal_diversity<std::uint64_t, std::uint8_t, 2>, "array"_a, "kernel"_a.noconvert());
        module.def("focal_diversity", focal_diversity<std::uint64_t, std::int32_t, 2>, "array"_a, "kernel"_a.noconvert());
        module.def("focal_diversity", focal_diversity<std::uint64_t, std::uint32_t, 2>, "array"_a, "kernel"_a.noconvert());
        module.def("focal_diversity", focal_diversity<std::uint64_t, std::int64_t, 2>, "array"_a, "kernel"_a.noconvert());
        module.def("focal_diversity", focal_diversity<std::uint64_t, std::uint64_t, 2>, "array"_a, "kernel"_a.noconvert());
    }

}  // namespace lue::framework

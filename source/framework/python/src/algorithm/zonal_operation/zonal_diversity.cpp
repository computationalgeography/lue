#include "lue/framework/algorithm/value_policies/zonal_diversity.hpp"
#include <pybind11/pybind11.h>


using namespace pybind11::literals;


namespace lue::framework {
    namespace {

        template<
            typename Count,
            typename Element,
            typename Zone,
            Rank rank>
        PartitionedArray<Count, rank> zonal_diversity(
            PartitionedArray<Element, rank> const& array,
            PartitionedArray<Zone, rank> const& zones)
        {
            return value_policies::zonal_diversity<Count>(array, zones);
        }

    }  // Anonymous namespace


    void bind_zonal_diversity(
        pybind11::module& module)
    {
        module.def("zonal_diversity", zonal_diversity<std::uint64_t, std::uint8_t, std::uint8_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_diversity", zonal_diversity<std::uint64_t, std::uint8_t, std::uint32_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_diversity", zonal_diversity<std::uint64_t, std::uint8_t, std::uint64_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_diversity", zonal_diversity<std::uint64_t, std::uint8_t, std::int32_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_diversity", zonal_diversity<std::uint64_t, std::uint8_t, std::int64_t, 2>, "array"_a, "zones"_a);

        module.def("zonal_diversity", zonal_diversity<std::uint64_t, std::int32_t, std::uint8_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_diversity", zonal_diversity<std::uint64_t, std::int32_t, std::uint32_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_diversity", zonal_diversity<std::uint64_t, std::int32_t, std::uint64_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_diversity", zonal_diversity<std::uint64_t, std::int32_t, std::int32_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_diversity", zonal_diversity<std::uint64_t, std::int32_t, std::int64_t, 2>, "array"_a, "zones"_a);

        module.def("zonal_diversity", zonal_diversity<std::uint64_t, std::uint32_t, std::uint8_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_diversity", zonal_diversity<std::uint64_t, std::uint32_t, std::uint32_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_diversity", zonal_diversity<std::uint64_t, std::uint32_t, std::uint64_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_diversity", zonal_diversity<std::uint64_t, std::uint32_t, std::int32_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_diversity", zonal_diversity<std::uint64_t, std::uint32_t, std::int64_t, 2>, "array"_a, "zones"_a);

        module.def("zonal_diversity", zonal_diversity<std::uint64_t, std::int64_t, std::uint8_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_diversity", zonal_diversity<std::uint64_t, std::int64_t, std::uint32_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_diversity", zonal_diversity<std::uint64_t, std::int64_t, std::uint64_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_diversity", zonal_diversity<std::uint64_t, std::int64_t, std::int32_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_diversity", zonal_diversity<std::uint64_t, std::int64_t, std::int64_t, 2>, "array"_a, "zones"_a);

        module.def("zonal_diversity", zonal_diversity<std::uint64_t, std::uint64_t, std::uint8_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_diversity", zonal_diversity<std::uint64_t, std::uint64_t, std::uint32_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_diversity", zonal_diversity<std::uint64_t, std::uint64_t, std::uint64_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_diversity", zonal_diversity<std::uint64_t, std::uint64_t, std::int32_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_diversity", zonal_diversity<std::uint64_t, std::uint64_t, std::int64_t, 2>, "array"_a, "zones"_a);
    }

}  // namespace lue::framework

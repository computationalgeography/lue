#include "lue/framework/algorithm/value_policies/zonal_maximum.hpp"
#include <pybind11/numpy.h>


using namespace pybind11::literals;


namespace lue::framework {
    namespace {

        template<
            typename Element,
            typename Zone,
            Rank rank>
        PartitionedArray<Element, rank> zonal_maximum(
            PartitionedArray<Element, rank> const& array,
            PartitionedArray<Zone, rank> const& zones)
        {
            return value_policies::zonal_maximum(array, zones);
        }

    }  // Anonymous namespace


    void bind_zonal_maximum(
        pybind11::module& module)
    {
        module.def("zonal_maximum", zonal_maximum<std::uint8_t, std::uint8_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_maximum", zonal_maximum<std::uint8_t, std::uint32_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_maximum", zonal_maximum<std::uint8_t, std::uint64_t, 2>, "array"_a, "zones"_a);

        module.def("zonal_maximum", zonal_maximum<std::int32_t, std::uint8_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_maximum", zonal_maximum<std::int32_t, std::uint32_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_maximum", zonal_maximum<std::int32_t, std::uint64_t, 2>, "array"_a, "zones"_a);

        module.def("zonal_maximum", zonal_maximum<std::uint32_t, std::uint8_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_maximum", zonal_maximum<std::uint32_t, std::uint32_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_maximum", zonal_maximum<std::uint32_t, std::uint64_t, 2>, "array"_a, "zones"_a);

        module.def("zonal_maximum", zonal_maximum<std::int64_t, std::uint8_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_maximum", zonal_maximum<std::int64_t, std::uint32_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_maximum", zonal_maximum<std::int64_t, std::uint64_t, 2>, "array"_a, "zones"_a);

        module.def("zonal_maximum", zonal_maximum<std::uint64_t, std::uint8_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_maximum", zonal_maximum<std::uint64_t, std::uint32_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_maximum", zonal_maximum<std::uint64_t, std::uint64_t, 2>, "array"_a, "zones"_a);

        module.def("zonal_maximum", zonal_maximum<float, std::uint8_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_maximum", zonal_maximum<float, std::uint32_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_maximum", zonal_maximum<float, std::uint64_t, 2>, "array"_a, "zones"_a);

        module.def("zonal_maximum", zonal_maximum<double, std::uint8_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_maximum", zonal_maximum<double, std::uint32_t, 2>, "array"_a, "zones"_a);
        module.def("zonal_maximum", zonal_maximum<double, std::uint64_t, 2>, "array"_a, "zones"_a);
    }

}  // namespace lue::framework

#include "lue/framework/algorithm/value_policies/zonal_normal.hpp"
#include <pybind11/pybind11.h>


using namespace pybind11::literals;


namespace lue::framework {
    namespace {

        template<
            typename Element,
            typename Zone,
            Rank rank>
        PartitionedArray<Element, rank> zonal_normal(
            PartitionedArray<Zone, rank> const& zones)
        {
            return value_policies::zonal_normal<Element>(zones);
        }

    }  // Anonymous namespace


    void bind_zonal_normal(
        pybind11::module& module)
    {
        // TODO Add dtype argument
        // module.def("zonal_normal", zonal_normal<std::uint8_t, 2>, "zones"_a);
        // module.def("zonal_normal", zonal_normal<std::uint32_t, 2>, "zones"_a);
        // module.def("zonal_normal", zonal_normal<std::uint64_t, 2>, "zones"_a);
    }

}  // namespace lue::framework

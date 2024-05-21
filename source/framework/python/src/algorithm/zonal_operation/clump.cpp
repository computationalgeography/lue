#include "lue/framework/algorithm/value_policies/clump.hpp"
#include <pybind11/pybind11.h>


using namespace pybind11::literals;


namespace lue::framework {
    namespace {

        template<typename Zone, Rank rank>
        auto clump(PartitionedArray<Zone, rank> const& zones, Connectivity const connectivity)
            -> PartitionedArray<Zone, rank>
        {
            return value_policies::clump(zones, connectivity);
        }

    }  // Anonymous namespace


    void bind_clump(pybind11::module& module)
    {
        pybind11::enum_<Connectivity>(module, "Connectivity")
            .value("diagonal", Connectivity::diagonal)
            .value("nondiagonal", Connectivity::nondiagonal)
            // .export_values()
            ;

        module.def("clump", clump<std::uint8_t, 2>, "zones"_a);
        module.def("clump", clump<std::uint32_t, 2>, "zones"_a);
        module.def("clump", clump<std::uint64_t, 2>, "zones"_a);
        module.def("clump", clump<std::int32_t, 2>, "zones"_a);
        module.def("clump", clump<std::int64_t, 2>, "zones"_a);
    }

}  // namespace lue::framework

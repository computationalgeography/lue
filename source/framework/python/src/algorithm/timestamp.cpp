#include "lue/framework/algorithm/timestamp.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        Rank const rank{2};

        template<
            typename Element>
        PartitionedArray<ClockTick, rank> timestamp(
            PartitionedArray<Element, rank> const& array)
        {
            return lue::timestamp(array);
        }

    }  // Anonymous namespace


    void bind_timestamp(
        pybind11::module& module)
    {
        module.def("timestamp", timestamp<std::uint8_t>);
        module.def("timestamp", timestamp<std::uint32_t>);
        module.def("timestamp", timestamp<std::uint64_t>);
        module.def("timestamp", timestamp<std::int8_t>);
        module.def("timestamp", timestamp<std::int32_t>);
        module.def("timestamp", timestamp<std::int64_t>);
        module.def("timestamp", timestamp<float>);
        module.def("timestamp", timestamp<double>);
    }

}  // namespace lue::framework

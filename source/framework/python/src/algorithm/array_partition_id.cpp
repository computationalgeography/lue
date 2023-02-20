#include "lue/framework/algorithm/array_partition_id.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        Rank const rank{2};

        template<typename Element>
        PartitionedArray<std::uint64_t, rank> array_partition_id(PartitionedArray<Element, rank> const& array)
        {
            return lue::array_partition_id(array);
        }

    }  // Anonymous namespace


    void bind_array_partition_id(pybind11::module& module)
    {
        module.def("array_partition_id", array_partition_id<std::uint8_t>);
        module.def("array_partition_id", array_partition_id<std::uint32_t>);
        module.def("array_partition_id", array_partition_id<std::uint64_t>);
        module.def("array_partition_id", array_partition_id<std::int32_t>);
        module.def("array_partition_id", array_partition_id<std::int64_t>);
        module.def("array_partition_id", array_partition_id<float>);
        module.def("array_partition_id", array_partition_id<double>);
    }

}  // namespace lue::framework

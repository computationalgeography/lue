#include "lue/framework/algorithm/locality_id.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        Rank const rank{2};

        template<
            typename Element>
        PartitionedArray<std::uint32_t, rank> locality_id(
            PartitionedArray<Element, rank> const& array)
        {
            return lue::locality_id(array);
        }

    }  // Anonymous namespace


    void bind_locality_id(
        pybind11::module& module)
    {
        module.def("locality_id", locality_id<std::uint8_t>);
        module.def("locality_id", locality_id<std::uint32_t>);
        module.def("locality_id", locality_id<std::uint64_t>);
        module.def("locality_id", locality_id<std::int32_t>);
        module.def("locality_id", locality_id<std::int64_t>);
        module.def("locality_id", locality_id<float>);
        module.def("locality_id", locality_id<double>);
    }

}  // namespace lue::framework

#include "lue/framework/core/component.hpp"
#include "lue/framework/partitioned_array.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<typename Element, Rank rank>
        void wait(PartitionedArray<Element, rank> const& array)
        {
            wait_all(array);

            lue_hpx_assert(all_are_valid(array));
            lue_hpx_assert(all_are_ready(array));
        }

    }  // Anonymous namespace


    void bind_wait_partitioned_array(pybind11::module& module)
    {
        module.def("wait", wait<std::uint8_t, 2>);
        module.def("wait", wait<std::uint32_t, 2>);
        module.def("wait", wait<std::uint64_t, 2>);
        module.def("wait", wait<std::int32_t, 2>);
        module.def("wait", wait<std::int64_t, 2>);
        module.def("wait", wait<float, 2>);
        module.def("wait", wait<double, 2>);
    }
}  // namespace lue::framework

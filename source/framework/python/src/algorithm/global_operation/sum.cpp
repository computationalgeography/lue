#include "lue/framework/algorithm/value_policies/sum.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {
    namespace {

        template<typename Element, Rank rank>
        auto sum(PartitionedArray<Element, rank> const& array) -> Scalar<Element>
        {
            return value_policies::sum(array);
        }

    }  // Anonymous namespace


    void bind_sum(pybind11::module& module)
    {
        module.def("sum", sum<std::uint8_t, 2>);
        module.def("sum", sum<std::uint32_t, 2>);
        module.def("sum", sum<std::uint64_t, 2>);
        module.def("sum", sum<std::int32_t, 2>);
        module.def("sum", sum<std::int64_t, 2>);
        module.def("sum", sum<float, 2>);
        module.def("sum", sum<double, 2>);
    }

}  // namespace lue::framework

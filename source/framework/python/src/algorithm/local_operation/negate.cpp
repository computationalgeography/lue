#include "lue/py/framework/algorithm/negate.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {

    void bind_negate(pybind11::module& module)
    {
        module.def("negate", negate<std::int32_t, 2>);
        module.def("negate", negate<std::int64_t, 2>);
        module.def("negate", negate<float, 2>);
        module.def("negate", negate<double, 2>);
    }

}  // namespace lue::framework

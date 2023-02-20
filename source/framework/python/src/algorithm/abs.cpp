#include "lue/py/framework/algorithm/abs.hpp"
#include <pybind11/pybind11.h>


namespace lue::framework {

    void bind_abs(pybind11::module& module)
    {
        module.def("abs", abs<std::int32_t, 2>);
        module.def("abs", abs<std::int64_t, 2>);
        module.def("abs", abs<float, 2>);
        module.def("abs", abs<double, 2>);
    }

}  // namespace lue::framework

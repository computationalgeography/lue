#include <pybind11/pybind11.h>


namespace lue {
namespace framework {

void bind_add(pybind11::module& module);
void bind_maximum(pybind11::module& module);
void bind_minimum(pybind11::module& module);
void bind_sqrt(pybind11::module& module);
void bind_uniform(pybind11::module& module);


void bind_local_operations(
    pybind11::module& module)
{
    bind_add(module);
    bind_maximum(module);
    bind_minimum(module);
    bind_sqrt(module);
    bind_uniform(module);
}

}  // namespace framework
}  // namespace lue

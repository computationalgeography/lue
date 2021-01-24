#include "hpx_runtime.hpp"
#include "lue/py/framework/submodule.hpp"
#include <pybind11/stl.h>


namespace py = pybind11;
using namespace py::literals;


namespace lue {
namespace {

HPXRuntime* runtime = nullptr;


void start_hpx_runtime(std::vector<std::string> const& configuration)
{
    // Iff the pointer to the runtime is not pointing to an instance,
    // instantiate one. This will start the HPX runtime.
    if(runtime == nullptr)
    {
        py::gil_scoped_release release;
        runtime = new HPXRuntime(configuration);
    }
}


void stop_hpx_runtime()
{
    // Iff the pointer to the runtime is pointing to an instance, delete
    // it. This will stop the HPX runtime.
    if(runtime != nullptr)
    {
        HPXRuntime* r = runtime;
        runtime = nullptr;
        py::gil_scoped_release release;
        delete r;
    }
}


bool on_root_locality()
{
    return hpx::find_here() == hpx::find_root_locality();
}

}  // Anonymous namespace


namespace framework {

void init_local_operations(py::module& module);
void init_partitioned_array(py::module& module);


void init_submodule(
    py::module& module)
{
    py::module submodule = module.def_submodule(
        "framework",
        R"(
    :mod:`lue.framework` --- LUE Environmental Modelling Framework
    ==============================================================

    The :mod:`lue.framework` package ...
)");

    submodule.def(
        "start_hpx_runtime",
        &start_hpx_runtime);

    submodule.def(
        "stop_hpx_runtime",
        &stop_hpx_runtime);

    submodule.def(
        "on_root_locality",
        &on_root_locality);

    // Wrap high-level data structures
    init_partitioned_array(submodule);

    // Wrap high-level algorithms
    init_local_operations(submodule);

    // Unless the user calls stop_hpx_runtime explicitly, we will do it
    // automatically upon module unload
    auto atexit = py::module_::import("atexit");
    atexit.attr("register")(py::cpp_function(
        []() { stop_hpx_runtime(); }));
}

}  // namespace framework
}  // namespace lue

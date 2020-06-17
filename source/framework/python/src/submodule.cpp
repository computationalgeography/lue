#include "lue/py/framework/submodule.hpp"
#include <hpx/hpx_start.hpp>
#include <hpx/hpx_suspend.hpp>
#include <hpx/include/apply.hpp>
#include <pybind11/stl.h>
#include <fmt/format.h>


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue {
namespace framework {
namespace {

void start_hpx_runtime(
    std::vector<std::string> arguments)
{
    std::vector<char*> argv(arguments.size() + 1);
    std::transform(arguments.begin(), arguments.end(), argv.begin(),
            [arguments](std::string& argument)
            {
                return &argument[0];
            }
        );
    argv.back() = nullptr;

    // Initialize HPX, but don't run hpx_main
    hpx::start(nullptr, argv.size() - 1, argv.data());
}


void stop_hpx_runtime()
{
    // Schedule a finalize
    hpx::apply([]() { hpx::finalize(); });

    // Wait for tasks to finish and stop the runtime
    if(hpx::stop() != EXIT_SUCCESS)
    {
        throw std::runtime_error("Error while stopping the HPX runtime");
    }
}


void restart_hpx(
    std::vector<std::string> const& arguments)
{
    stop_hpx_runtime();
    start_hpx_runtime(arguments);
}


// void reconfigure_runtime(
//     unsigned int const nr_threads)
// {
//     std::vector<std::string> arguments{
//             fmt::format("--hpx:threads={}", nr_threads),
//         };
// 
//     restart_hpx(arguments);
// }


bool on_root_locality()
{
    {
        auto const localities{hpx::find_all_localities()};
        std::cout << "nr_localities: " << localities.size() << ": " << std::endl;
        for(auto const locality: localities) {
            std::cout << locality << " ";
        }
        std::cout << std::endl;

        std::cout << "here: " << hpx::find_here() << " " << hpx::get_locality_name() << std::endl;
        std::cout << "root: " << hpx::find_root_locality() << std::endl;

    }

    return hpx::find_here() == hpx::find_root_locality();

    // return hpx::get_locality_name() == "gransasso#0";
}


void initialize_module(
    py::module& /* module */)
{
    unsigned int const nr_threads{std::thread::hardware_concurrency()};

    std::vector<std::string> arguments{
            "lue",
            fmt::format("--hpx:threads={}", nr_threads),
        };

    start_hpx_runtime(arguments);

    // using namespace std::chrono_literals;
    // std::this_thread::sleep_for(2s);
}


void schedule_terminate_module(
    py::module& /* module */)
{
    auto atexit = py::module::import("atexit");
    atexit.attr("register")(py::cpp_function(&stop_hpx_runtime));
}

}  // Anonymous namespace


void init_local_operations(pybind11::module& module);
void init_partitioned_array(pybind11::module& module);


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

    initialize_module(submodule);
    schedule_terminate_module(submodule);

    submodule.def(
        "reconfigure_hpx",
        &restart_hpx);

    // submodule.def(
    //     "reconfigure_runtime",
    //     &reconfigure_runtime,
    //     "nr_threads"_a);

    submodule.def(
        "on_root_locality",
        &on_root_locality);

    // Wrap high-level data structures
    init_partitioned_array(submodule);

    // Wrap high-level algorithms
    init_local_operations(submodule);
}

}  // namespace framework
}  // namespace lue


// Script used for testing
// Missing piece is support for multi-locality runs. Can't yet initialize
// HPX runtime in such a way that they cooperate correctly.

// import lue.framework as lfr
// import numpy as np
// import os
// import sys
// import time
// 
// 
// def create_array(
//         values,
//         shape):
//     array = lfr.create_array(values, shape)
//     array = lfr.uniform(array, 1.0, 1000.0)
// 
//     return array
// 
// 
// # def create_array_1d():
// #     values = np.arange(1e8, dtype=np.float64)
// #     shape = (1000000)
// # 
// #     return create_array(values, shape)
// 
// 
// def create_array_2d():
//     values = np.arange(1e8, dtype=np.float64).reshape(10000, 10000)
//     shape = (1000, 1000)
// 
//     return create_array(values, shape)
// 
// 
// def iterate(state):
//     for i in range(40):
//         sys.stdout.write("iterate{}\n".format(i)); sys.stdout.flush()
//         state = lfr.sqrt(state)
//         sys.stdout.write("/iterate{}\n".format(i)); sys.stdout.flush()
// 
//     return state
// 
// 
// def simulate(nr_threads):
//     state = iterate(create_array_2d())
//     print(state)
// 
// 
// arguments = sys.argv + [
//         '--hpx:ini=hpx.agas.max_pending_refcnt_requests!=400'
//     ]
// 
// print("arguments: {}".format(arguments))
// 
// # time.sleep(2)
// lfr.reconfigure_hpx(arguments)
// # time.sleep(2)
// 
// print("arguments: {}\non root locality: {}".format(arguments, lfr.on_root_locality()))
// 
// # if lfr.on_root_locality():
// # 
// #     simulate(nr_threads=2)
// 
// 
// #     # simulate(nr_threads=1)
// #     # simulate(nr_threads=3)
// #     # simulate(nr_threads=5)
// #     #     return fmt::format("thread:0-{}=core:0-{}.pu:0", nr_threads-1, nr_threads-1);


// Script used for starting multiple localities
// #!/usr/bin/env bash
// set -e
// 
// 
// nr_localities=3
// nr_threads_per_locality=2
// 
// $LUE_OBJECTS/_deps/hpx-build/bin/hpxrun.py --localities=$nr_localities --thread=$nr_threads_per_locality --verbose `which python` -- $LUE/lue_py_example.py

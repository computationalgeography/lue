#include "lue/qa/scalability/array_experiment.hpp"
#include "lue/qa/scalability/io.hpp"
#include "lue/qa/scalability/run.hpp"
#include "lue/py/qa/submodule.hpp"


namespace py = pybind11;
using namespace pybind11::literals;


namespace lue::qa {

    void init_submodule(
        py::module& module)
    {
        py::module submodule = module.def_submodule("qa");

        py::class_<Experiment>(
            submodule,
            "Experiment",
            R"(
    Class for instances containing information about a scalability experiment.
)")
            .def(
                    "add",
                    &Experiment::add,
                    "run"_a
                )

            ;

        py::class_<ArrayExperiment, Experiment>(
            submodule,
            "ArrayExperiment",
            R"(
    Class for instances containing information about an array scalability
    experiment.
)")
            .def(
                    py::init<Count, Shape const&, Shape const&>(),
                    "nr_workers"_a,
                    "array_shape"_a,
                    "partition_shape"_a
                )
            .def(
                    "start",
                    &Run::start
                )
            .def(
                    "stop",
                    &Run::stop
                )
            ;

        py::class_<Run>(
            submodule,
            "Run",
            R"(
    Class for keeping track of information of running some calculations once.
)")
            .def(
                    py::init<>())
            .def(
                    "start",
                    &Run::start
                )
            .def(
                    "stop",
                    &Run::stop
                )
            ;


        submodule.def(
                "save_results",
                save_results,
                "experiment"_a,
                "pathname"_a
            );

    }

}  // namespace lue::qa

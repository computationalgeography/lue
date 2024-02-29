#include "lue/py/qa/submodule.hpp"
#include "lue/qa/scalability/array_experiment.hpp"
#include "lue/qa/scalability/io.hpp"
#include "lue/qa/scalability/run.hpp"


using namespace pybind11::literals;


namespace lue::qa {

    void init_submodule(pybind11::module& module)
    {
        pybind11::module submodule = module.def_submodule("qa");

        pybind11::class_<Experiment>(
            submodule,
            "Experiment",
            R"(
    Class for instances containing information about a scalability experiment.
)")
            .def("add", &Experiment::add, "run"_a)

            ;

        pybind11::class_<ArrayExperiment, Experiment>(
            submodule,
            "ArrayExperiment",
            R"(
    Class for instances containing information about an array scalability
    experiment.
)")
            .def(
                pybind11::init<Count, Shape const&, Shape const&>(),
                "nr_workers"_a,
                "array_shape"_a,
                "partition_shape"_a)
            .def("start", &Run::start)
            .def("stop", &Run::stop);

        pybind11::class_<Run>(
            submodule,
            "Run",
            R"(
    Class for keeping track of information of running some calculations once.
)")
            .def(pybind11::init<>())
            .def("start", &Run::start)
            .def("stop", &Run::stop);


        submodule.def("save_results", save_results, "experiment"_a, "pathname"_a);
    }

}  // namespace lue::qa

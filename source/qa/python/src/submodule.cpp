#include "lue/py/qa/submodule.hpp"
#include "lue/qa/scalability/instrument/array_experiment.hpp"
#include "lue/qa/scalability/instrument/io.hpp"
#include "lue/qa/scalability/instrument/run.hpp"


using namespace pybind11::literals;


namespace lue::qa {

    void init_submodule(pybind11::module& module)
    {
        pybind11::module qa_module = module.def_submodule("qa");

        pybind11::module scalability_module = qa_module.def_submodule(
            "scalability",
            R"(
    lue.qa.scalability
    ==================
)");

        pybind11::module instrument_module = scalability_module.def_submodule(
            "instrument",
            R"(
    lue.qa.scalability.instrument
    =============================
)");

        pybind11::class_<Run>(
            instrument_module,
            "Run",
            R"(
    Class for keeping track of information of running some calculations once.
)")
            .def(pybind11::init<>())
            .def(
                "start",
                &Run::start,
                R"(
    Start the run
)")
            .def(
                "stop",
                &Run::stop,
                R"(
    Stop the run
)");

        pybind11::class_<Experiment, Run>(
            instrument_module,
            "Experiment",
            R"(
    Class for instances containing information about scalability experiments

    :param int nr_workers: Number of workers
)")
            .def(pybind11::init<Count>(), "nr_workers"_a)
            .def_property_readonly("nr_workers", &Experiment::nr_workers)
            .def(
                "add",
                &Experiment::add,
                "run"_a,
                R"(
    Add the results of an experimental run
)");

        pybind11::class_<ArrayExperiment, Experiment>(
            instrument_module,
            "ArrayExperiment",
            R"(
    Class for instances containing information about a scalability experiments involving (2D) partitioned
    arrays

    :param int nr_workers: Number of workers
    :param tuple array_shape: Shape of the array
    :param tuple partition_shape: Shape of the array partitions
)")
            .def(
                pybind11::init<Count, Shape const&, Shape const&>(),
                "nr_workers"_a,
                "array_shape"_a,
                "partition_shape"_a)
            .def_property_readonly("array_shape", &ArrayExperiment::array_shape)
            .def_property_readonly("partition_shape", &ArrayExperiment::partition_shape);

        instrument_module.def("save_results", save_results, "experiment"_a, "pathname"_a);
    }

}  // namespace lue::qa

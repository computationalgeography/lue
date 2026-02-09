#include "lue/framework/model/progressor.hpp"
#include <pybind11/pybind11.h>


using namespace pybind11::literals;


namespace lue::framework {

    class PyProgressor: public Progressor, public pybind11::trampoline_self_life_support
    {

        public:

            // Inherit constructors
            using Progressor::Progressor;


            void preprocess(Count const sample_nr) override
            {
                PYBIND11_OVERRIDE(void, Progressor, preprocess, sample_nr);
            }


            void initialize() override
            {
                PYBIND11_OVERRIDE(void, Progressor, initialize, );
            }


            void simulate(Count const time_step) override
            {
                PYBIND11_OVERRIDE(void, Progressor, simulate, time_step);
            }


            void finalize() override
            {
                PYBIND11_OVERRIDE(void, Progressor, finalize, );
            }


            void postprocess() override
            {
                PYBIND11_OVERRIDE(void, Progressor, postprocess, );
            }
    };


    void bind_progressor(pybind11::module& module)
    {
        pybind11::class_<Progressor, PyProgressor, pybind11::smart_holder>(module, "Progressor")
            .def(pybind11::init<>())
            .def("preprocess", &Progressor::preprocess)
            .def("initialize", &Progressor::initialize)
            .def("simulate", &Progressor::simulate)
            .def("finalize", &Progressor::finalize)
            .def("postprocess", &Progressor::postprocess);
    }

}  // namespace lue::framework

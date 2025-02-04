#include "lue/framework/model/model.hpp"
#include <pybind11/pybind11.h>


using namespace pybind11::literals;


namespace lue::framework {

    class PyModel: public Model
    {

        public:

            // Inherit constructors
            using Model::Model;


            void preprocess(Count const sample_nr) override
            {
                PYBIND11_OVERRIDE(void, Model, preprocess, sample_nr);
            }


            void initialize() override
            {
                PYBIND11_OVERRIDE(void, Model, initialize);
            }


            auto simulate(Count const time_step) -> hpx::shared_future<void> override
            {
                PYBIND11_OVERRIDE(hpx::shared_future<void>, Model, simulate, time_step);
            }


            void finalize() override
            {
                PYBIND11_OVERRIDE(void, Model, finalize);
            }


            void postprocess() override
            {
                PYBIND11_OVERRIDE(void, Model, postprocess);
            }
    };


    void bind_model(pybind11::module& module)
    {
        pybind11::class_<Model, PyModel>(module, "Model")
            .def(pybind11::init<>())
            .def("preprocess", &Model::preprocess)
            .def("initialize", &Model::initialize)
            .def("simulate", &Model::simulate)
            .def("finalize", &Model::finalize)
            .def("postprocess", &Model::postprocess);
    }

}  // namespace lue::framework

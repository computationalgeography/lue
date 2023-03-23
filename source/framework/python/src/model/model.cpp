#include "lue/framework/model/model.hpp"
#include <pybind11/pybind11.h>


using namespace pybind11::literals;


// namespace PYBIND11_NAMESPACE { namespace detail {
//
//     template <> struct type_caster<hpx::shared_future<void>>
//     {
//         public:
//
//             /**
//              * This macro establishes the name 'inty' in
//              * function signatures and declares a local variable
//              * 'value' of type inty
//              */
//             PYBIND11_TYPE_CASTER(hpx::shared_future<void>, const_name("inty"));
//
//             /**
//              * Conversion part 1 (Python->C++): convert a PyObject into a inty
//              * instance or return false upon failure. The second argument
//              * indicates whether implicit conversions should be applied.
//              */
//             bool load(handle src, bool) {
//                 /* Extract PyObject from handle */
//                 PyObject *source = src.ptr();
//                 /* Try converting into a Python integer value */
//                 PyObject *tmp = PyNumber_Long(source);
//                 if (!tmp)
//                     return false;
//                 /* Now try to convert into a C++ int */
//                 value.long_value = PyLong_AsLong(tmp);
//                 Py_DECREF(tmp);
//                 /* Ensure return code was OK (to avoid out-of-range errors etc) */
//                 return !(value.long_value == -1 && !PyErr_Occurred());
//             }
// // //
// // //         /**
// // //          * Conversion part 2 (C++ -> Python): convert an inty instance into
// // //          * a Python object. The second and third arguments are used to
// // //          * indicate the return value policy and parent object (for
// // //          * ``return_value_policy::reference_internal``) and are generally
// // //          * ignored by implicit casters.
// // //          */
// // //         static handle cast(inty src, return_value_policy /* policy */, handle /* parent */) {
// // //             return PyLong_FromLong(src.long_value);
// // //         }
// // //     };
// // //
// // // }} // namespace PYBIND11_NAMESPACE::detail


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


            hpx::shared_future<void> simulate(Count const time_step) override
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

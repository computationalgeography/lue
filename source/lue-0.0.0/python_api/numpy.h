#pragma once
#include <hdf5.h>
// #include <Python.h>
#include <pybind11/pybind11.h>
#include <tuple>


namespace lue {
namespace python {

#if PY_MAJOR_VERSION >= 3
#define DEFINE_INIT_NUMPY()  \
static void* init_numpy()    \
{                            \
    import_array();          \
    return NULL;             \
}
#else
#define DEFINE_INIT_NUMPY()  \
static void init_numpy()     \
{                            \
    import_array();          \
}
#endif


std::tuple<hid_t, hid_t>
                   numpy_type_to_hdf5_types(int const type_id);

pybind11::object   hdf5_type_id_to_numpy_dtype(
                                        hid_t const type_id);

}  // namespace python
}  // namespace lue

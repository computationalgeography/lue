#pragma once
#include <Python.h>


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

}  // namespace python
}  // namespace lue

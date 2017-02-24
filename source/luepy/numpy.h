#pragma once
#include <numpy/arrayobject.h>


namespace lue {

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

}  // namespace lue

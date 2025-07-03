#pragma once
#include <numpy/arrayobject.h>


namespace lue {

#define DEFINE_INIT_NUMPY()                                                                                  \
    static void* init_numpy()                                                                                \
    {                                                                                                        \
        import_array();                                                                                      \
        return NULL;                                                                                         \
    }

}  // namespace lue

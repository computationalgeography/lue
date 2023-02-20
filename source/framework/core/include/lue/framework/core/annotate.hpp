#pragma once
#include <hpx/config/defines.hpp>
#ifdef HPX_HAVE_APEX
#include <hpx/threading_base/annotated_function.hpp>
#endif


namespace lue {

#ifdef HPX_HAVE_APEX
    using AnnotateFunction = hpx::scoped_annotation;
#else
#define AnnotateFunction [[maybe_unused]] static char const*
#endif

}  // namespace lue

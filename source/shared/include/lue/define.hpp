#pragma once


// FIXME
// Sometimes it is not possible to use the [[maybe_unused]]
// attribute. This is (currently) the case with structured bindings.
// Starting with GCC8 this should be possible. Once LUE requires C++20
// when can likely get rid of this macro and mark unused structured bindings
// as [[maybe_unused]]
// https://stackoverflow.com/questions/41404001/structured-binding-with-maybe-unused
#define LUE_UNUSED(expr)                                                                                     \
    do                                                                                                       \
    {                                                                                                        \
        (void)(expr);                                                                                        \
    } while (0)


namespace lue {

}  // namespace lue

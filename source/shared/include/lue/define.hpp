#pragma once


#define LUE_UNUSED(expr)                                                                                     \
    /* NOLINTBEGIN(cppcoreguidelines-avoid-do-while) */                                                      \
    do                                                                                                       \
    {                                                                                                        \
        (void)(expr);                                                                                        \
    } while (0) /* NOLINTEND(cppcoreguidelines-avoid-do-while) */


namespace lue {

}  // namespace lue

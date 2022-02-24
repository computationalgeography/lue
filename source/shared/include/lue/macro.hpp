#pragma once


// At least with Visual Studio, passing arguments to a macro with a double set of parentheses
// results in a syntax error. This macro removes a set of parentheses.
#define ESC(...) __VA_ARGS__


namespace lue {
    namespace detail {

        template<
            typename T>
        struct ArgumentType;

        template<
            typename T,
            typename U>
        struct ArgumentType<T(U)>
        {
            using Argument = U;
        };

    }  // namespace detail


    template<
        typename T>
    using ArgumentType = typename detail::ArgumentType<T>::Argument;

}  // namespace lue

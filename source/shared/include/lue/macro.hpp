#pragma once


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

#pragma once


namespace lue {

    template<typename Functor>
    using OutputElementT = typename Functor::OutputElement;


    template<typename Functor>
    using AggregatorT = typename Functor::Aggregator;


    template<class SomeType>
    class FunctorTraits
    {

        public:

            static constexpr bool const is_functor{false};
    };


    template<typename Functor>
    constexpr bool is_functor_v = FunctorTraits<Functor>::is_functor;

}  // namespace lue

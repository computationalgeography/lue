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

            static constexpr char const* name{SomeType::name};
    };


    template<typename Functor>
    constexpr char const* functor_name = FunctorTraits<Functor>::name;

}  // namespace lue

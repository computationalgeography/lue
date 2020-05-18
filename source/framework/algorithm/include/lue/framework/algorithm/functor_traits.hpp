#pragma once


namespace lue {

template<
    typename Functor>
using OutputElementT = typename Functor::OutputElement;


template<
    typename Functor>
using AggregatorT = typename Functor::Aggregator;

}  // namespace lue

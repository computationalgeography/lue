#pragma once


namespace lue {

template<
    typename Value>
class ValueTraits
{
};


template<
    typename Value>
using ElementT = typename ValueTraits<Value>::Element;

}  // namespace lue

#pragma once


namespace lue::data_model {

    template<typename Value>
    class ValueTraits
    {
    };


    template<typename Value>
    using ElementT = typename ValueTraits<Value>::Element;

}  // namespace lue::data_model

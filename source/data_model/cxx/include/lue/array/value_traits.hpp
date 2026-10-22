#pragma once


namespace lue::data_model {

    template<typename Value>
    class ValueTraits
    {
    };


    template<typename Value>
    using ElementT = ValueTraits<Value>::Element;

}  // namespace lue::data_model

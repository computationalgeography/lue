#pragma once


namespace lue {
    namespace data_model {

        template<typename Value>
        class ValueTraits
        {
        };


        template<typename Value>
        using ElementT = typename ValueTraits<Value>::Element;

    }  // namespace data_model
}  // namespace lue

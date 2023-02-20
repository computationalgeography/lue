#pragma once


namespace lue {
    namespace data_model {

        template<typename Property>
        class PropertyTraits
        {
        };


        template<typename Property>
        using ValueT = typename PropertyTraits<Property>::Value;

    }  // namespace data_model
}  // namespace lue

#pragma once


namespace lue::data_model {

    template<typename Property>
    class PropertyTraits
    {
    };


    template<typename Property>
    using ValueT = typename PropertyTraits<Property>::Value;

}  // namespace lue::data_model

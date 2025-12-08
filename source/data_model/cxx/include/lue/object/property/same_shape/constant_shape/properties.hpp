#pragma once
#include "lue/core/collection.hpp"
#include "lue/info/property/same_shape/constant_shape/property.hpp"
#include "lue/object/property/properties_traits.hpp"


namespace lue::data_model {
    namespace same_shape::constant_shape {

        /*!
            @brief      Collection of same shape x constant shape properties
        */
        class LUE_DATA_MODEL_EXPORT Properties: public Collection<Property>
        {

            public:

                explicit Properties(hdf5::Group const& parent);

                explicit Properties(Collection<Property>&& collection);

                Properties(Properties const& other) = default;

                Properties(Properties&& other) = default;

                ~Properties() override = default;

                auto operator=(Properties const& other) -> Properties& = default;

                auto operator=(Properties&& other) -> Properties& = default;

                auto add(
                    std::string const& name,
                    hdf5::Datatype const& datatype,
                    std::string const& description = "") -> Property&;

                auto add(
                    std::string const& name,
                    hdf5::Datatype const& datatype,
                    hdf5::Shape const& shape,
                    std::string const& description = "") -> Property&;

            private:
        };


        auto create_properties(hdf5::Group& parent) -> Properties;

    }  // namespace same_shape::constant_shape


    template<>
    class PropertyTraits<same_shape::constant_shape::Properties>
    {

        public:

            using Property = same_shape::constant_shape::Properties::Element;

            using Value = ValueT<Property>;
    };

}  // namespace lue::data_model

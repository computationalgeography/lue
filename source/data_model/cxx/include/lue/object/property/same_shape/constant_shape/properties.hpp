#pragma once
#include "lue/core/collection.hpp"
#include "lue/info/property/same_shape/constant_shape/property.hpp"
#include "lue/object/property/properties_traits.hpp"


namespace lue {
    namespace data_model {
        namespace same_shape {
            namespace constant_shape {

                /*!
                    @brief      Collection of same shape x constant shape properties
                */
                class Properties: public Collection<Property>
                {

                    public:

                        explicit Properties(hdf5::Group const& parent);

                        explicit Properties(Collection<Property>&& collection);

                        Properties(Properties const&) = default;

                        Properties(Properties&&) = default;

                        ~Properties() override = default;

                        Properties& operator=(Properties const&) = default;

                        Properties& operator=(Properties&&) = default;

                        Property& add(
                            std::string const& name,
                            hdf5::Datatype const& datatype,
                            std::string const& description = "");

                        Property& add(
                            std::string const& name,
                            hdf5::Datatype const& datatype,
                            hdf5::Shape const& shape,
                            std::string const& description = "");

                    private:
                };


                Properties create_properties(hdf5::Group& parent);

            }  // namespace constant_shape
        }      // namespace same_shape


        template<>
        class PropertyTraits<same_shape::constant_shape::Properties>
        {

            public:

                using Property = same_shape::constant_shape::Properties::Element;

                using Value = ValueT<Property>;
        };

    }  // namespace data_model
}  // namespace lue

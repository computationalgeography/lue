#pragma once
#include "lue/array/same_shape/constant_shape/value.hpp"
#include "lue/info/property/property_group.hpp"
#include "lue/info/property/property_traits.hpp"


namespace lue {
    namespace data_model {
        namespace same_shape {
            namespace constant_shape {

                class Property: public PropertyGroup
                {

                    public:

                        using Value = same_shape::constant_shape::Value;

                        Property(hdf5::Group const& parent, std::string const& name);

                        Property(
                            hdf5::Group const& parent,
                            std::string const& name,
                            hdf5::Datatype const& memory_datatype);

                        Property(PropertyGroup&& group, Value&& value);

                        Property(Property const&) = default;

                        Property(Property&&) = default;

                        ~Property() override = default;

                        Property& operator=(Property const&) = default;

                        Property& operator=(Property&&) = default;

                        Value const& value() const;

                        Value& value();

                    private:

                        Value _value;
                };


                Property create_property(
                    hdf5::Group& parent,
                    std::string const& name,
                    hdf5::Datatype const& memory_datatype,
                    std::string const& description = "");

                Property create_property(
                    hdf5::Group& parent,
                    std::string const& name,
                    hdf5::Datatype const& memory_datatype,
                    hdf5::Shape const& array_shape,
                    std::string const& description = "");

                Property create_property(
                    hdf5::Group& parent,
                    std::string const& name,
                    hdf5::Datatype const& file_datatype,
                    hdf5::Datatype const& memory_datatype,
                    std::string const& description = "");

                Property create_property(
                    hdf5::Group& parent,
                    std::string const& name,
                    hdf5::Datatype const& file_datatype,
                    hdf5::Datatype const& memory_datatype,
                    hdf5::Shape const& array_shape,
                    std::string const& description = "");

            }  // namespace constant_shape
        }      // namespace same_shape


        template<>
        class PropertyTraits<same_shape::constant_shape::Property>
        {

            public:

                using Value = same_shape::constant_shape::Property::Value;
        };

    }  // namespace data_model
}  // namespace lue

#pragma once
#include "lue/array/same_shape/constant_shape/value.hpp"
#include "lue/info/property/property_group.hpp"
#include "lue/info/property/property_traits.hpp"


namespace lue::data_model {
    namespace same_shape::constant_shape {

        class LUE_DATA_MODEL_EXPORT Property: public PropertyGroup
        {

            public:

                using Value = same_shape::constant_shape::Value;

                Property(hdf5::Group const& parent, std::string const& name);

                Property(
                    hdf5::Group const& parent,
                    std::string const& name,
                    hdf5::Datatype const& memory_datatype);

                Property(PropertyGroup&& group, Value&& value);

                Property(Property const& other) = default;

                Property(Property&& other) = default;

                ~Property() override = default;

                auto operator=(Property const& other) -> Property& = default;

                auto operator=(Property&& other) -> Property& = default;

                auto value() const -> Value const&;

                auto value() -> Value&;

            private:

                Value _value;
        };


        auto create_property(
            hdf5::Group& parent,
            std::string const& name,
            hdf5::Datatype const& memory_datatype,
            std::string const& description = "") -> Property;

        auto create_property(
            hdf5::Group& parent,
            std::string const& name,
            hdf5::Datatype const& memory_datatype,
            hdf5::Shape const& array_shape,
            std::string const& description = "") -> Property;

        auto create_property(
            hdf5::Group& parent,
            std::string const& name,
            hdf5::Datatype const& file_datatype,
            hdf5::Datatype const& memory_datatype,
            std::string const& description = "") -> Property;

        auto create_property(
            hdf5::Group& parent,
            std::string const& name,
            hdf5::Datatype const& file_datatype,
            hdf5::Datatype const& memory_datatype,
            hdf5::Shape const& array_shape,
            std::string const& description = "") -> Property;

    }  // namespace same_shape::constant_shape


    template<>
    class PropertyTraits<same_shape::constant_shape::Property>
    {

        public:

            using Value = same_shape::constant_shape::Property::Value;
    };

}  // namespace lue::data_model

#pragma once
#include "lue/array/same_shape/variable_shape/value.hpp"
#include "lue/info/property/property_group.hpp"
#include "lue/info/property/property_traits.hpp"


namespace lue::data_model {
    namespace same_shape::variable_shape {

        class LUE_DATA_MODEL_EXPORT Property: public PropertyGroup
        {

            public:

                using Value = same_shape::variable_shape::Value;

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
            Rank rank,
            std::string const& description = "") -> Property;

        auto create_property(
            hdf5::Group& parent,
            std::string const& name,
            hdf5::Datatype const& file_datatype,
            hdf5::Datatype const& memory_datatype,
            Rank rank,
            std::string const& description = "") -> Property;

    }  // namespace same_shape::variable_shape


    template<>
    class PropertyTraits<same_shape::variable_shape::Property>
    {

        public:

            using Value = same_shape::variable_shape::Property::Value;
    };

}  // namespace lue::data_model

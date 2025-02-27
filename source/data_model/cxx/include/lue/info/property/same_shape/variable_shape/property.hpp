#pragma once
#include "lue/array/same_shape/variable_shape/value.hpp"
#include "lue/info/property/property_group.hpp"
#include "lue/info/property/property_traits.hpp"


namespace lue {
    namespace data_model {
        namespace same_shape {
            namespace variable_shape {

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
                    Rank rank,
                    std::string const& description = "");

                Property create_property(
                    hdf5::Group& parent,
                    std::string const& name,
                    hdf5::Datatype const& file_datatype,
                    hdf5::Datatype const& memory_datatype,
                    Rank rank,
                    std::string const& description = "");

            }  // namespace variable_shape
        }  // namespace same_shape


        template<>
        class PropertyTraits<same_shape::variable_shape::Property>
        {

            public:

                using Value = same_shape::variable_shape::Property::Value;
        };

    }  // namespace data_model
}  // namespace lue

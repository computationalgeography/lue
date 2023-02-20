#include "lue/info/property/same_shape/variable_shape/property.hpp"
#include "lue/core/tag.hpp"


namespace lue {
    namespace data_model {
        namespace same_shape {
            namespace variable_shape {

                Property::Property(hdf5::Group const& parent, std::string const& name):

                    PropertyGroup{parent, name},
                    _value{*this, value_tag}

                {
                }


                Property::Property(
                    hdf5::Group const& parent,
                    std::string const& name,
                    hdf5::Datatype const& memory_datatype):

                    PropertyGroup{parent, name},
                    _value{*this, value_tag, memory_datatype}

                {
                }


                Property::Property(PropertyGroup&& group, Value&& value):

                    PropertyGroup{std::move(group)},
                    _value{std::move(value)}

                {
                }


                Property::Value const& Property::value() const
                {
                    return _value;
                }


                Property::Value& Property::value()
                {
                    return _value;
                }


                Property create_property(
                    hdf5::Group& parent,
                    std::string const& name,
                    hdf5::Datatype const& memory_datatype,
                    Rank const rank,
                    std::string const& description)
                {
                    return create_property(
                        parent, name, file_datatype(memory_datatype), memory_datatype, rank, description);
                }


                Property create_property(
                    hdf5::Group& parent,
                    std::string const& name,
                    hdf5::Datatype const& file_datatype,
                    hdf5::Datatype const& memory_datatype,
                    Rank const rank,
                    std::string const& description)
                {
                    PropertyGroup group{create_property_group(parent, name, description)};
                    Value value{create_value(group, value_tag, file_datatype, memory_datatype, rank)};

                    return Property{std::move(group), std::move(value)};
                }

            }  // namespace variable_shape
        }      // namespace same_shape
    }          // namespace data_model
}  // namespace lue

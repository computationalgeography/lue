#pragma once
#include "lue/array/different_shape/value.hpp"
#include "lue/array/value_group.hpp"


namespace lue {
    namespace data_model {
        namespace different_shape {
            namespace variable_shape {

                /*!
                    @brief      Class for storing different shape x variable value x variable
                                shape object arrays

                    The implementation uses an HDF5 group, containing an HDF5 group for
                    each location in time. Each of these groups contains an HDF5 dataset
                    for each active object.
                */
                class Value: public ValueGroup
                {

                    public:

                        Value(hdf5::Group const& parent, std::string const& name);

                        Value(
                            hdf5::Group const& parent,
                            std::string const& name,
                            hdf5::Datatype const& memory_datatype);

                        explicit Value(ValueGroup&& group);

                        Value(Value const&) = default;

                        Value(Value&&) = default;

                        ~Value() override = default;

                        Value& operator=(Value const&) = default;

                        Value& operator=(Value&&) = default;

                        Count nr_locations_in_time() const;

                        different_shape::Value expand(
                            Index idx, Count nr_objects, ID const* ids, hdf5::Shape const* shapes);

                        different_shape::Value operator[](Index idx);

                    private:

                        Count _nr_locations_in_time;
                };


                Value create_value(
                    hdf5::Group& parent,
                    std::string const& name,
                    hdf5::Datatype const& memory_datatype,
                    Rank rank);

                Value create_value(
                    hdf5::Group& parent,
                    std::string const& name,
                    hdf5::Datatype const& file_datatype,
                    hdf5::Datatype const& memory_datatype,
                    Rank rank);

            }  // namespace variable_shape
        }      // namespace different_shape
    }          // namespace data_model
}  // namespace lue

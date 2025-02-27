#pragma once
#include "lue/array/same_shape/value.hpp"
#include "lue/array/value_group.hpp"


namespace lue {
    namespace data_model {
        namespace same_shape {
            namespace variable_shape {

                /*!
                    @brief      Class for storing same shape x variable value x variable
                                shape object arrays

                    The implementation uses an HDF5 group with for each location in time
                    an HDF5 dataset with the object arrays of the active objects.
                */
                class LUE_DATA_MODEL_EXPORT Value: public ValueGroup
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

                        same_shape::Value expand(Index idx, Count nr_arrays, hdf5::Shape const& array_shape);

                        same_shape::Value operator[](Index idx);

                    private:

                        Count _nr_locations_in_time;
                };


                LUE_DATA_MODEL_EXPORT Value create_value(
                    hdf5::Group& parent,
                    std::string const& name,
                    hdf5::Datatype const& memory_datatype,
                    Rank rank);

                LUE_DATA_MODEL_EXPORT Value create_value(
                    hdf5::Group& parent,
                    std::string const& name,
                    hdf5::Datatype const& file_datatype,
                    hdf5::Datatype const& memory_datatype,
                    Rank rank);

            }  // namespace variable_shape
        }  // namespace same_shape
    }  // namespace data_model
}  // namespace lue

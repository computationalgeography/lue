#pragma once
#include "lue/array/same_shape/value.hpp"


namespace lue {
namespace same_shape {
namespace variable_shape {

/*!
    @brief      Class for storing same shape x variable value x variable
                shape object arrays

    The implementation uses an HDF5 group with for each location in time
    an HDF5 dataset with the object arrays of the active objects.
*/
class Value:
    public hdf5::Group
{

public:

                   Value               (hdf5::Group const& parent,
                                        std::string const& name);

                   Value               (hdf5::Group const& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype);

                   Value               (Group&& group,
                                        hdf5::Datatype const& memory_datatype);

                   Value               (Value const&)=delete;

                   Value               (Value&&)=default;

                   ~Value              ()=default;

    Value&         operator=           (Value const&)=delete;

    Value&         operator=           (Value&&)=default;

    hdf5::Datatype const&
                   file_datatype       () const;

    hdf5::Datatype const&
                   memory_datatype     () const;

    Rank           rank                () const;

    Count          nr_locations_in_time() const;

    same_shape::Value
                   reserve             (Index idx,
                                        Count nr_arrays,
                                        hdf5::Shape const& array_shape);

    same_shape::Value
                   operator[]          (Index idx);

private:

    //! In-file datatype
    hdf5::Datatype const _file_datatype;

    //! In-memory datatype
    hdf5::Datatype const _memory_datatype;

    //! Rank of the object arrays
    Rank const     _rank;

    Count          _nr_locations_in_time;

};


Value              create_value        (hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype,
                                        Rank rank);

Value              create_value        (hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype,
                                        Rank rank);

}  // namespace variable_shape
}  // namespace same_shape
}  // namespace lue

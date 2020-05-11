#pragma once
#include "lue/array/value_group.hpp"
#include "lue/array/same_shape/constant_shape/value.hpp"


namespace lue {
namespace data_model {
namespace different_shape {
namespace constant_shape {

/*!
    @brief      Class for storing different shape x constant shape
                object arrays

    The implementation uses an HDF5 dataset per object. This dataset has
    one dimension more than the rank of the object arrays. This first
    dimension represents the time. The first location in time for which
    an object array is stored for an object is stored at the first index
    in the object's corresponding HDF5 dataset, etc.

    As an example, when storing stacks of rasters for one or more time
    boxes, one way to do that is to store a 3D object array per time
    box and discretize this value using time and space discretization
    properties. To read or write a whole singe raster from the HDF5
    dataset you need to setup a hyperslab as follows:
    - offset:
        - time box index [0, nr_time_boxes)
        - time step index [0, nr_time_steps)
        - 0
        - 0
    - count:
        - 1
        - 1
        - number of rows
        - number of columns
*/
class Value:
    public ValueGroup
{

public:

                   Value               (hdf5::Group& parent,
                                        std::string const& name);

                   Value               (hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype);

    explicit       Value               (ValueGroup&& group);

                   Value               (Value const&)=delete;

                   Value               (Value&&)=default;

                   ~Value              () override =default;

    Value&         operator=           (Value const&)=delete;

    Value&         operator=           (Value&&)=default;

    Count          nr_objects          () const;

    bool           exists              (ID id) const;

    void           expand              (Count nr_objects,
                                        ID const* ids,
                                        hdf5::Shape const* shapes,
                                        Count const* nr_locations_in_time);

    same_shape::constant_shape::Value
                   expand              (ID id,
                                        hdf5::Shape const& shape,
                                        Count nr_locations_in_time);

    same_shape::constant_shape::Value
                   operator[]          (ID id);

private:

    same_shape::constant_shape::Value
                   expand_             (ID id,
                                        hdf5::Shape const& shape,
                                        Count nr_locations_in_time);

    Count          _nr_objects;

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

}  // namespace constant_shape
}  // namespace different_shape
}  // namespace data_model
}  // namespace lue

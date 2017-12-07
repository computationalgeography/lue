#pragma once
// #include "lue/constant_size/value.hpp"
#include "lue/array.hpp"
#include "lue/hdf5/group.hpp"


namespace lue {
namespace constant_size {
namespace variable {

/*!
    @brief      SameShape manages variable item values for constant
                sized collections of values with the same shape

    The underlying HDF5 dataset has two dimensions more than the rank of the
    individual values. The first dimension represents the time domain items
    and the second represents the items.
*/
class SameShape:
    public Array,
    public constant_size::SameShape
{

public:

                   SameShape           (hdf5::Group const& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype);

                   SameShape               (hdf5::Dataset&& dataset,
                                        hdf5::Datatype const& memory_datatype);

                   SameShape               (SameShape const&)=delete;

                   SameShape               (SameShape&&)=default;

                   ~SameShape              ()=default;

    SameShape&         operator=           (SameShape const&)=delete;

    SameShape&         operator=           (SameShape&&)=default;

    void           reserve             (hsize_t nr_time_domain_items,
                                        hsize_t nr_items);

    hsize_t        nr_items            () const final;

    hdf5::Shape    value_shape         () const;

private:

};


SameShape              create_value        (hdf5::Group const& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype);

SameShape              create_value        (hdf5::Group const& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype,
                                        hdf5::Shape const& value_shape);

}  // namespace variable
}  // namespace constant_size
}  // namespace lue

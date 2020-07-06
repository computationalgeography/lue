#pragma once
#include "lue/core/array.hpp"
#include "lue/core/index_range.hpp"
#include "lue/hdf5.hpp"


namespace lue {
namespace data_model {
namespace same_shape {

/*!
    @brief      Class for storing same shape x constant value object arrays
    @todo       Refactor the implementation with
                same_shape::constant_shape::Value

    The implementation uses an HDF5 dataset with one dimension more than
    the rank of the individual object arrays. This first dimension
    represents the objects.
*/
class Value:
    public Array
{

public:

                   Value               (hdf5::Group const& parent,
                                        std::string const& name);

                   Value               (hdf5::Group const& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype);

    explicit       Value               (Array&& array);

                   Value               (Value const&)=default;

                   Value               (Value&&)=default;

                   ~Value              () override =default;

    Value&         operator=           (Value const&)=default;

    Value&         operator=           (Value&&)=default;

    void           expand              (Count nr_arrays);

    Count          nr_arrays           () const;

    hdf5::Shape    array_shape         () const;

    using Array::read;

    void           read                (Index idx,
                                        void* buffer);

    void           read                (IndexRange const& range,
                                        void* buffer);

    using Array::write;

    void           write               (Index idx,
                                        void const* buffer);

    void           write               (IndexRange const& range,
                                        void const* buffer);

private:

    hdf5::Hyperslab hyperslab          (IndexRange const& range) const;

    hdf5::Hyperslab hyperslab          (Index idx) const;

};


Value              create_value        (hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype);

Value              create_value        (hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype,
                                        hdf5::Shape const& array_shape);

Value              create_value        (hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype);

Value              create_value        (hdf5::Group& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype,
                                        hdf5::Shape const& array_shape);

}  // namespace same_shape
}  // namespace data_model
}  // namespace lue

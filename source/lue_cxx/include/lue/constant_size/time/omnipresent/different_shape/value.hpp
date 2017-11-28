#pragma once
#include "lue/constant_size/time/omnipresent/value.hpp"
#include "lue/hdf5/group.hpp"
#include "lue/array.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {
namespace different_shape {

/*!
    The property value is represented by a dataset per item. All datasets
    have the same rank, but the size of each dimension may be different.
*/
class Value:
    public hdf5::Group,
    public omnipresent::Value
{

public:

                   Value               (hdf5::Group const& parent,
                                        std::string const& name);

                   Value               (hdf5::Group const& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype);
// 
//                    // Value               (hdf5::Identifier const& location,
//                    //                      std::string const& name,
//                    //                      hid_t const type_id);
// 
//                    // Value               (hdf5::Identifier&& location,
//                    //                      hid_t const type_id);

                   Value               (hdf5::Group&& group,
                                        hdf5::Datatype const& memory_datatype);

                   Value               (Value const&)=delete;

                   Value               (Value&&)=default;

                   ~Value              ()=default;

    Value&         operator=           (Value const&)=delete;

    Value&         operator=           (Value&&)=default;

    hsize_t        nr_items            () const final;

    int            rank                () const;

    hdf5::Datatype const&
                   file_datatype       () const;

    hdf5::Datatype const&
                   memory_datatype     () const;

    void           reserve             (hsize_t nr_items,
                                        hsize_t const* shapes);

    Array          operator[]          (size_t idx) const;

private:

    hsize_t        _nr_items;

    int            _rank;

    hdf5::Datatype const _file_datatype;

    hdf5::Datatype const _memory_datatype;

    void           reserve_value       (hsize_t idx,
                                        hsize_t const* shape);

};


Value              create_value        (hdf5::Group const& parent,
                                        std::string const& name,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype,
                                        int rank);
// 
// Value              create_value        (hdf5::Identifier const& location,
//                                         std::string const& name,
//                                         hdf5::Datatype const& file_datatype,
//                                         hdf5::Datatype const& memory_datatype,
//                                         hdf5::Shape const& value_shape,
//                                         hdf5::Shape const& value_chunk);

}  // namespace different_shape
}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue

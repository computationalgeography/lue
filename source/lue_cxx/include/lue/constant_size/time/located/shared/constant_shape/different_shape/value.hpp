#pragma once
#include "lue/constant_size/value.hpp"
#include "lue/array.hpp"
#include "lue/hdf5/group.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace located {
namespace shared {
namespace constant_shape {
namespace different_shape {

/*!
    The property value is represented by a dataset per item. All datasets
    have the same rank, but the size of each dimension may be different.
*/
class Value:
    public hdf5::Group,
    public constant_size::Value
{

public:

                   Value               (hdf5::Identifier const& location,
                                        std::string const& name);

                   Value               (hdf5::Identifier const& location,
                                        std::string const& name,
                                        hdf5::Datatype const& memory_datatype);

                   Value               (hdf5::Group&& group,
                                        hdf5::Datatype const& memory_datatype);

                   Value               (Value const& other)=delete;

                   Value               (Value&& other)=default;

                   ~Value              ()=default;

    Value&         operator=           (Value const& other)=delete;

    Value&         operator=           (Value&& other)=default;

    hsize_t        nr_items            () const final;

    int            rank                () const;

    hdf5::Datatype const&
                   file_datatype       () const;

    hdf5::Datatype const&
                   memory_datatype     () const;

    void           reserve             (hsize_t const nr_time_domain_items,
                                        hsize_t const nr_items,
                                        hsize_t const* shapes);

    Array          operator[]          (size_t const idx) const;

private:

    hsize_t        _nr_items;

    int            _rank;

    hdf5::Datatype const _file_datatype;

    hdf5::Datatype const _memory_datatype;

    void           reserve_value       (hsize_t const idx,
                                        hsize_t const nr_time_domain_items,
                                        hsize_t const* shape);

};


Value              create_value        (hdf5::Identifier const& location,
                                        std::string const& name,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype,
                                        int const rank);

}  // namespace different_shape
}  // namespace constant_shape
}  // namespace shared
}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue

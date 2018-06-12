#pragma once
#include "lue/item/same_shape/constant_shape/constant_collection/synchronous_value.hpp"
#include "lue/clock.hpp"


namespace lue {

/*!
    @brief      .
    @param      .
    @return     .
    @exception  .

    - Zero or more time time domain items for the collection of items
        as a whole
    - Each time domain item has a unique location in time
*/
class SynchronousTimeDomainItem:
    public same_shape::constant_shape::constant_collection::SynchronousValue
{

public:

                   SynchronousTimeDomainItem(
                                        hdf5::Group& parent);

                   SynchronousTimeDomainItem(
                                        SynchronousTimeDomainItem const&)=delete;

                   SynchronousTimeDomainItem(
                                        SynchronousTimeDomainItem&&)=default;

                   ~SynchronousTimeDomainItem()=default;

    SynchronousTimeDomainItem&
                   operator=           (SynchronousTimeDomainItem const&)=delete;

    SynchronousTimeDomainItem&
                   operator=           (SynchronousTimeDomainItem&&)=default;

    void           reserve             (hsize_t nr_time_domain_items);

private:

    using Base =
        same_shape::constant_shape::constant_collection::SynchronousValue;

};


SynchronousTimeDomainItem create_synchronous_time_domain_item(
                                        hdf5::Group& parent,
                                        hdf5::Datatype const& file_datatype,
                                        hdf5::Datatype const& memory_datatype,
                                        hdf5::Shape const& value_shape);

}  // namespace lue

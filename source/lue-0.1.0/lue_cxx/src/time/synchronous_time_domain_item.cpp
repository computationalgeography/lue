#include "lue/time/synchronous_time_domain_item.hpp"
#include "lue/tag.hpp"


namespace lue {

SynchronousTimeDomainItem::SynchronousTimeDomainItem(
    hdf5::Group& parent)

    : Base{
        parent, coordinates_tag,
        hdf5::Datatype{
            hdf5::NativeDatatypeTraits<time::DurationCount>::type_id()}}

{
}


void SynchronousTimeDomainItem::reserve(
    hsize_t nr_time_domain_items)
{
    Base::reserve(1, nr_time_domain_items);
}


SynchronousTimeDomainItem create_synchronous_time_domain_item(
    hdf5::Group& parent,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype,
    hdf5::Shape const& value_shape)
{
    same_shape::constant_shape::constant_collection::create_synchronous_value(
        parent, coordinates_tag, file_datatype, memory_datatype, value_shape);

    return SynchronousTimeDomainItem{parent};
}

}  // namespace lue

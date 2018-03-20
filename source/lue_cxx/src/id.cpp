#include "lue/id.hpp"
#include "lue/tag.hpp"


namespace lue {

Id::Id(
    hdf5::Group& parent)

    : same_shape::constant_shape::ContinuousValue{
        parent, ids_tag, hdf5::Datatype{H5T_NATIVE_HSIZE}}

{
}


Id create_id(
    hdf5::Group& parent)
{
    same_shape::constant_shape::create_continuous_value(
        parent, ids_tag, hdf5::Datatype{H5T_STD_U64LE},
        hdf5::Datatype{H5T_NATIVE_HSIZE});

    return Id{parent};
}

}  // namespace lue

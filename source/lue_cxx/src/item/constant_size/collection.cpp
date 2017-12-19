#include "lue/item/constant_size/collection.hpp"
#include "lue/tag.hpp"


namespace lue {
namespace constant_size {
namespace different_shape {

/*!
    @brief      Create group @a name in @a parent

    The datatype is of the individual values. The @a rank passed in
    defines the dimensionality of the item values.
*/
hdf5::Group create_group(
    hdf5::Group const& parent,
    std::string const& name,
    hdf5::Datatype const& memory_datatype,
    int const rank)
{
    return create_group(
        parent, name, hdf5::file_datatype(memory_datatype), memory_datatype,
        rank);
}


/*!
    @brief      Create group @a name in @a parent

    The datatypes are of the individual values. The @a rank passed in
    defines the dimensionality of the item values.
*/
hdf5::Group create_group(
    hdf5::Group const& parent,
    std::string const& name,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype,
    int const rank)
{
    auto group = hdf5::create_group(parent, name);

    group.attributes().write<std::vector<unsigned char>>(
        datatype_tag, hdf5::encode_datatype(file_datatype));
    group.attributes().write<int>(rank_tag, rank);
    group.attributes().write<hsize_t>(nr_items_tag, 0);

    return group;
}

}  // namespace different_shape
}  // namespace constant_size
}  // namespace lue

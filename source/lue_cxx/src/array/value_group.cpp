#include "lue/array/value_group.hpp"
#include "lue/core/tag.hpp"


namespace lue {

ValueGroup::ValueGroup(
    hdf5::Group& parent,
    std::string const& name)
:
    hdf5::Group{parent, name},
    _file_datatype{hdf5::decode_datatype(
        attributes().read<std::vector<unsigned char>>(datatype_tag))},
    _memory_datatype{hdf5::memory_datatype(_file_datatype)},
    _rank{attributes().read<Rank>(rank_tag)}

{
}


ValueGroup::ValueGroup(
    hdf5::Group& parent,
    std::string const& name,
    hdf5::Datatype const& memory_datatype)
:
    hdf5::Group{parent, name},
    _file_datatype{hdf5::decode_datatype(
        attributes().read<std::vector<unsigned char>>(datatype_tag))},
    _memory_datatype{memory_datatype},
    _rank{attributes().read<Rank>(rank_tag)}

{
}


ValueGroup::ValueGroup(
    Group&& group,
    hdf5::Datatype const& memory_datatype)
:
    Group{std::forward<hdf5::Group>(group)},
    _file_datatype{hdf5::decode_datatype(
        attributes().read<std::vector<unsigned char>>(datatype_tag))},
    _memory_datatype{memory_datatype},
    _rank{attributes().read<Rank>(rank_tag)}

{
}


hdf5::Datatype const& ValueGroup::file_datatype() const
{
    return _file_datatype;
}


hdf5::Datatype const& ValueGroup::memory_datatype() const
{
    return _memory_datatype;
}


Rank ValueGroup::rank() const
{
    return _rank;
}


ValueGroup create_value_group(
    hdf5::Group& parent,
    std::string const& name,
    hdf5::Datatype const& memory_datatype,
    Rank const rank)
{
    return create_value_group(
        parent, name,
        file_datatype(memory_datatype), memory_datatype,
        rank);
}


ValueGroup create_value_group(
    hdf5::Group& parent,
    std::string const& name,
    hdf5::Datatype const& file_datatype,
    hdf5::Datatype const& memory_datatype,
    Rank const rank)
{
    auto group = hdf5::create_group(parent, name);

    group.attributes().write<std::vector<unsigned char>>(
        datatype_tag, hdf5::encode_datatype(file_datatype));
    group.attributes().write<Rank>(rank_tag, rank);

    return ValueGroup{std::move(group), memory_datatype};
}

}  // namespace lue

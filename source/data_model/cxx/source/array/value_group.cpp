#include "lue/array/value_group.hpp"
#include "lue/core/tag.hpp"
#include <utility>


namespace lue::data_model {

    ValueGroup::ValueGroup(hdf5::Group const& parent, std::string const& name):

        hdf5::Group{parent, name},
        _file_datatype{hdf5::decode_datatype(attributes().read<std::vector<unsigned char>>(datatype_tag))},
        _memory_datatype{hdf5::memory_datatype(_file_datatype)},
        _rank{attributes().read<Rank>(rank_tag)}

    {
    }


    ValueGroup::ValueGroup(
        hdf5::Group const& parent, std::string const& name, hdf5::Datatype memory_datatype):

        hdf5::Group{parent, name},
        _file_datatype{hdf5::decode_datatype(attributes().read<std::vector<unsigned char>>(datatype_tag))},
        _memory_datatype{std::move(memory_datatype)},
        _rank{attributes().read<Rank>(rank_tag)}

    {
    }


    ValueGroup::ValueGroup(Group const& group, hdf5::Datatype memory_datatype):

        hdf5::Group{group},
        _file_datatype{hdf5::decode_datatype(attributes().read<std::vector<unsigned char>>(datatype_tag))},
        _memory_datatype{std::move(memory_datatype)},
        _rank{attributes().read<Rank>(rank_tag)}

    {
    }


    auto ValueGroup::file_datatype() const -> hdf5::Datatype const&
    {
        return _file_datatype;
    }


    auto ValueGroup::memory_datatype() const -> hdf5::Datatype const&
    {
        return _memory_datatype;
    }


    auto ValueGroup::rank() const -> Rank
    {
        return _rank;
    }


    auto create_value_group(
        hdf5::Group& parent, std::string const& name, hdf5::Datatype const& memory_datatype, Rank const rank)
        -> ValueGroup
    {
        return create_value_group(parent, name, file_datatype(memory_datatype), memory_datatype, rank);
    }


    auto create_value_group(
        hdf5::Group& parent,
        std::string const& name,
        hdf5::Datatype const& file_datatype,
        hdf5::Datatype const& memory_datatype,
        Rank const rank) -> ValueGroup
    {
        hdf5::Group group{hdf5::create_group(parent, name)};

        group.attributes().write<std::vector<unsigned char>>(
            datatype_tag, hdf5::encode_datatype(file_datatype));
        group.attributes().write<Rank>(rank_tag, rank);

        return ValueGroup{group, memory_datatype};
    }

}  // namespace lue::data_model

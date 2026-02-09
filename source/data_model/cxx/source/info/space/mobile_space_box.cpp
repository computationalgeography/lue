#include "lue/info/space/mobile_space_box.hpp"
#include "lue/core/tag.hpp"


namespace lue::data_model {

    MobileSpaceBox::MobileSpaceBox(hdf5::Group const& parent):

        same_shape::constant_shape::Value{parent, coordinates_tag}

    {
    }


    MobileSpaceBox::MobileSpaceBox(hdf5::Group const& parent, hdf5::Datatype const& memory_datatype):

        same_shape::constant_shape::Value{parent, coordinates_tag, memory_datatype}

    {
    }


    MobileSpaceBox::MobileSpaceBox(same_shape::constant_shape::Value&& value):

        same_shape::constant_shape::Value{std::move(value)}

    {
    }


    auto MobileSpaceBox::nr_boxes() const -> Count
    {
        return nr_arrays();
    }


    auto create_mobile_space_box(
        hdf5::Group& parent, hdf5::Datatype const& memory_datatype, std::size_t const rank) -> MobileSpaceBox
    {
        return create_mobile_space_box(parent, file_datatype(memory_datatype), memory_datatype, rank);
    }


    auto create_mobile_space_box(
        hdf5::Group& parent,
        hdf5::Datatype const& file_datatype,
        hdf5::Datatype const& memory_datatype,
        std::size_t const rank) -> MobileSpaceBox
    {
        // A box is defined by the coordinates of two opposite points
        // (diagonally). Two of them is enough.
        hdf5::Shape const value_shape{2 * rank};

        same_shape::constant_shape::Value value{same_shape::constant_shape::create_value(
            parent, coordinates_tag, file_datatype, memory_datatype, value_shape)};

        return MobileSpaceBox{std::move(value)};
    }

}  // namespace lue::data_model

#pragma once
#include "lue/cxx_api/constant_size/time/omnipresent/item.h"
#include "lue/cxx_api/array.h"
#include "lue/cxx_api/chunks.h"
#include <memory>


namespace lue {
namespace time {
namespace omnipresent {
namespace same_shape {

/*!
    @ingroup    lue_cxx_api_group

    The item is represented by a dataset where the first dimension
    represents the items and the subsequent dimensions represent the value
    per item.
*/
class Item:
    // public omnipresent::Item,
    public Array
{

public:

                   Item                (hdf5::Identifier const& location,
                                        std::string const& name);

                   Item                (hdf5::Identifier const& location,
                                        std::string const& name,
                                        hid_t const type_id);

                   Item                (hdf5::Identifier&& location,
                                        hid_t const type_id);

                   Item                (hdf5::Dataset&& dataset,
                                        hid_t const type_id);

                   Item                (Item const& other)=delete;

                   Item                (Item&& other)=default;

                   ~Item               ()=default;

    Item&          operator=           (Item const& other)=delete;

    Item&          operator=           (Item&& other)=default;

    void           reserve_items       (hsize_t const nr_items);

private:

};


Item               create_item         (hdf5::Identifier const& location,
                                        std::string const& name,
                                        hid_t const file_type_id,
                                        hid_t const memory_type_id);

Item               create_item         (hdf5::Identifier const& location,
                                        std::string const& name,
                                        hid_t const file_type_id,
                                        hid_t const memory_type_id,
                                        Shape const& shape,
                                        Chunks const& chunks);

}  // namespace same_shape
}  // namespace omnipresent
}  // namespace time
}  // namespace lue

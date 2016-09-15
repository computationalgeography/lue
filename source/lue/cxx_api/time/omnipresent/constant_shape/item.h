#pragma once
#include "lue/cxx_api/time/omnipresent/item.h"
// // #include "lue/cxx_api/time/omnipresent/space/omnipresent/property.h"
// #include "lue/cxx_api/array.h"
// #include "lue/cxx_api/value.h"
// #include "lue/cxx_api/data_type/time/property.h"
// // #include "lue/c_api/define.h"
// #include <memory>


namespace lue {
namespace omnipresent {
namespace constant_shape {

/*!
    @ingroup    lue_cxx_api_group
*/
class Item:
    lue::omnipresent::Item
{

public:

                   Item                (hdf5::Identifier const& location,
                                        std::string const& name);
                                        // hid_t const type_id
                                        // TypeId const type_id,
                                        // Shape const& shape,
                                        // Chunks const& chunks);

                   Item                (hdf5::Identifier&& location);

                   Item                (Item const& other)=delete;

                   Item                (Item&& other)=default;

                   ~Item               ()=default;

    Item&          operator=           (Item const& other)=delete;

    Item&          operator=           (Item&& other)=default;

    // Array&         reserve_items       (hsize_t const nr_items);

    // Array&         values              ();

private:

    // std::unique_ptr<Value> _value;

    // std::unique_ptr<Array> _items;

};


Item               create_item         (hdf5::Identifier const& location,
                                        std::string const& name);

}  // namespace constant_shape
}  // namespace omnipresent
}  // namespace lue

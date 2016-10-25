#pragma once
#include "lue/cxx_api/time/omnipresent/item.h"
#include "lue/cxx_api/hdf5/group.h"
#include "lue/cxx_api/array.h"
#include "lue/cxx_api/define.h"
// #include "lue/cxx_api/chunks.h"
// #include <memory>


namespace lue {
namespace time {
namespace omnipresent {
namespace different_shape {

/*!
    @ingroup    lue_cxx_api_group

    The item is represented by a dataset per item. All datasets have the same
    rank, but the size of each dimension may be different.
*/
class Item:
    // public omnipresent::Item
    public hdf5::Group
{

public:

                   Item                (hdf5::Group&& group);

                   Item                (hdf5::Identifier const& location,
                                        std::string const& name);

                   // Item                (hdf5::Identifier const& location,
                   //                      std::string const& name,
                   //                      hid_t const type_id);

                   // Item                (hdf5::Identifier&& location);
                                        // hid_t const type_id);

                   Item                (Item const& other)=delete;

                   Item                (Item&& other)=default;

                   ~Item               ()=default;

    Item&          operator=           (Item const& other)=delete;

    Item&          operator=           (Item&& other)=default;

    hid_t          type_id             () const;

    rank_t         rank                () const;

    void           reserve_items       (count_t const nr_items,
                                        extent_t const* shapes);

    count_t        nr_items            () const;

    Array          operator[]          (index_t const idx) const;

private:

    //! In-file type of individual values.
    hid_t const    _type_id;

    rank_t const   _rank;

    count_t        _nr_items;

    void           reserve_item        (count_t const idx,
                                        extent_t const* shape);

};


Item               create_item         (hdf5::Identifier const& location,
                                        std::string const& name,
                                        hid_t const type_id,
                                        rank_t const rank);

}  // namespace different_shape
}  // namespace omnipresent
}  // namespace time
}  // namespace lue

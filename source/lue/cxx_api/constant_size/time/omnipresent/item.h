#pragma once
#include "lue/cxx_api/constant_size/item.h"


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {

/*!
    @ingroup    lue_cxx_api_group
*/
class Item:
    constant_size::Item
{

public:

                   ~Item               ()=default;

protected:

                   Item                ();

                   // Item                (hdf5::Identifier const& location,
                   //                      std::string const& name);

                   // Item                (hdf5::Identifier&& location);

                   Item                (Item const& other)=delete;

                   Item                (Item&& other)=default;

    Item&          operator=           (Item const& other)=delete;

    Item&          operator=           (Item&& other)=default;

private:

};

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue

#pragma once
#include "lue/cxx_api/time/item.h"


namespace lue {
namespace omnipresent {

/*!
    @ingroup    lue_cxx_api_group
*/
class Item:
    lue::Item
{

public:

                   ~Item               ()=default;

protected:

                   Item                (hdf5::Identifier const& location,
                                        std::string const& name);

                   Item                (hdf5::Identifier&& location);

                   Item                (Item const& other)=delete;

                   Item                (Item&& other)=default;

    Item&          operator=           (Item const& other)=delete;

    Item&          operator=           (Item&& other)=default;

private:

};

}  // namespace omnipresent
}  // namespace lue

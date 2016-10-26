#pragma once
// #include "lue/cxx_api/hdf5/group.h"


namespace lue {
namespace constant_size {
namespace time {

// bool               item_exists         (hdf5::Identifier const& location,
//                                         std::string const& name);


/*!
    @ingroup    lue_cxx_api_group

    Base class for more specialized item classes. An item is a group
    containing information per item. Examples are:
    - an nD value per item
    - a polygon per item
    - a time point per item

    How to store this information efficiently depends on the time domain
    configuration and the information to store. Aspects influencing the
    storage strategy are:
    - Whether the information changes over time
    - In case the information changes over time, whether this happens at
      the same time points for different items
    - Whether the shape of the nD Value is different for different items

    Combining these aspects results in different storage strategies
    ranging from simple datasets to a dataset per time point and per item.
*/
class Item // :
    // public hdf5::Group
{

public:

    virtual        ~Item               ()=default;

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


// hdf5::Identifier   create_item         (hdf5::Identifier const& location,
//                                         std::string const& name);

}  // namespace time
}  // namespace constant_size
}  // namespace lue

#pragma once
#include "lue/cxx_api/array.h"
#include "lue/cxx_api/value.h"
#include "lue/cxx_api/time/property.h"
#include <memory>


namespace lue {
namespace time {
namespace omnipresent {
namespace variable_shape {

/*!
    @ingroup    lue_cxx_api_group
*/
class Property:
    time::Property
{

public:

                   Property            (lue::Property& group,
                                        hid_t const type_id);

                   Property            (Property const& other)=delete;

                   Property            (Property&& other)=default;

                   ~Property           ()=default;

    Property&      operator=           (Property const& other)=delete;

    Property&      operator=           (Property&& other)=default;

    // Array&         reserve_items       (hsize_t const nr_items);

    // Array&         values              ();

private:

    // std::unique_ptr<Value> _value;

    // std::unique_ptr<Array> _items;

};


void               configure_property  (lue::Property const& location,
                                        hid_t const type_id);

}  // namespace variable_shape
}  // namespace omnipresent
}  // namespace time
}  // namespace lue

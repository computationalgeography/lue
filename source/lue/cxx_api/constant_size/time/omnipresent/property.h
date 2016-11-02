#pragma once
#include "lue/cxx_api/constant_size/time/property.h"


namespace lue {
namespace constant_size {
namespace time {
namespace omnipresent {

/*!
    @ingroup    lue_cxx_api_group
*/
class Property:
    public time::Property
{

public:

                   Property            (lue::Property& group);

                   Property            (Property const& other)=delete;

                   Property            (Property&& other)=default;

                   ~Property           ()=default;

    Property&      operator=           (Property const& other)=delete;

    Property&      operator=           (Property&& other)=default;

private:

};

}  // namespace omnipresent
}  // namespace time
}  // namespace constant_size
}  // namespace lue

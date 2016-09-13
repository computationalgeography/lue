#pragma once
#include "lue/cxx_api/chunks.h"
#include "lue/cxx_api/define.h"
#include "lue/cxx_api/property.h"
#include "lue/cxx_api/shape.h"
#include <functional>


namespace lue {
namespace api {

/*!
    @ingroup    lue_cxx_api_group
*/
class Property
{

public:

    virtual        ~Property           ()=default;

    hdf5::Identifier const&
                   id                  () const;

    std::string    name                () const;

protected:

                   Property            (lue::Property& group);

                   Property            (Property const& other)=delete;

                   Property            (Property&& other)=default;

    Property&      operator=           (Property const& other)=delete;

    Property&      operator=           (Property&& other)=default;

    lue::Property const&
                   group               () const;

    lue::Property&
                   group               ();

private:

    std::reference_wrapper<lue::Property> _group;

};

} // namespace api
} // namespace lue

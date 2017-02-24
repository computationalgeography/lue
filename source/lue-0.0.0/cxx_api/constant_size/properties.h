#pragma once
#include "lue/cxx_api/properties.h"
// #include <functional>


namespace lue {
namespace constant_size {

/*!
    @ingroup    lue_cxx_api_group
*/
template<
    typename Property>
class Properties:
    public lue::Properties<Property>
{

public:

                   Properties          (lue::Properties<>& group);

                   Properties          (Properties<Property> const& other)
                                            =delete;

                   Properties          (Properties<Property>&& other)=default;

                   ~Properties         ()=default;

    Properties<Property>&
                   operator=           (Properties<Property> const& other)
                                            =delete;

    Properties<Property>&
                   operator=           (Properties<Property>&& other)=default;

private:

    // std::reference_wrapper<lue::Properties<>> _group;

};


template<
    typename Property>
inline Properties<Property>::Properties(
    lue::Properties<>& group)

    : lue::Properties<Property>(group.id())  // ,
      // _group(std::ref(group))

{
}

}  // namespace constant_size
}  // namespace lue

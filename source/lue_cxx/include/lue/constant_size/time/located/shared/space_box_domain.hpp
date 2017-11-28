#pragma once
#include "lue/constant_size/time/located/shared/property_set.hpp"
#include "lue/constant_size/time/located/shared/space_box.hpp"
#include "lue/constant_size/time/located/shared/space_domain.hpp"


namespace lue {
namespace constant_size {
namespace time {
namespace located {
namespace shared {

class SpaceBoxDomain:
    public SpaceDomain
{

public:

    static hdf5::Datatype file_datatype(hdf5::Group const& parent);

                   SpaceBoxDomain      (SpaceDomain&& space_domain);

                   SpaceBoxDomain      (SpaceDomain&& space_domain,
                                        hdf5::Datatype const& memory_datatype);

                   SpaceBoxDomain      (SpaceBoxDomain const& other)=delete;

                   SpaceBoxDomain      (SpaceBoxDomain&& other)=default;

                   ~SpaceBoxDomain     ()=default;

    SpaceBoxDomain& operator=          (SpaceBoxDomain const& other)=delete;

    SpaceBoxDomain& operator=          (SpaceBoxDomain&& other)=default;

    SpaceBox const& items              () const;

    SpaceBox&      items               ();

    SpaceBox&      reserve             (hsize_t const nr_items);

private:

    SpaceBox       _items;

};


SpaceBoxDomain     create_space_box_domain(
                                        PropertySet& property_set,
                                        hdf5::Datatype const file_datatype,
                                        hdf5::Datatype const memory_datatype,
                                        size_t const rank);

}  // namespace shared
}  // namespace located
}  // namespace time
}  // namespace constant_size
}  // namespace lue

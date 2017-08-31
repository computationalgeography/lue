#pragma once
#include "lue/hdf5/group.h"


namespace lue {

class SpaceDomain:
    public hdf5::Group
{

public:

                   SpaceDomain         (hdf5::Group const& group);

                   SpaceDomain         (hdf5::Group&& group);

                   SpaceDomain         (SpaceDomain const& other);

                   SpaceDomain         (SpaceDomain&& other)=default;

                   ~SpaceDomain        ()=default;

    SpaceDomain&   operator=           (SpaceDomain const& other)=delete;

    SpaceDomain&   operator=           (SpaceDomain&& other)=default;

};


SpaceDomain        create_space_domain (hdf5::Group const& group);

}  // namespace lue

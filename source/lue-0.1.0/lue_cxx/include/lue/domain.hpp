#pragma once
#include "lue/hdf5/group.hpp"


namespace lue {

class Domain:
    public hdf5::Group
{

public:

                   Domain              (hdf5::Group const& parent);

                   Domain              (hdf5::Group&& group);

                   Domain              (Domain const&)=delete;

                   Domain              (Domain&&)=default;

    virtual        ~Domain             ()=default;

    Domain&        operator=           (Domain const&)=delete;

    Domain&        operator=           (Domain&&)=default;

private:

};


Domain             create_domain       (hdf5::Group& parent);

} // namespace lue

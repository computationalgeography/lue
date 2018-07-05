#pragma once
#include "lue/hdf5/link.hpp"


namespace lue {
namespace hdf5 {

class HardLink:
    public Link
{

public:

                   HardLink            (Group& group,
                                        std::string const& name);

                   HardLink            (Group const&)=delete;

                   HardLink            (HardLink&&)=default;

                   ~HardLink           ()=default;

    HardLink&      operator=           (HardLink const&)=delete;

    HardLink&      operator=           (HardLink&&)=default;

};


bool               hard_link_exists    (Group const& group,
                                        std::string const& name);

HardLink           create_hard_link    (Group& group,
                                        Identifier const& target,
                                        std::string const& name);

} // namespace hdf5
} // namespace lue

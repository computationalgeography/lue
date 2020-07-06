#pragma once
#include "lue/hdf5/link.hpp"


namespace lue {
namespace hdf5 {

class SoftLink:
    public Link
{

public:

                   SoftLink            (Group const& group,
                                        std::string const& name);

                   SoftLink            (SoftLink const&)=default;

                   SoftLink            (SoftLink&&)=default;

                   ~SoftLink           ()=default;

    SoftLink&      operator=           (SoftLink const&)=default;

    SoftLink&      operator=           (SoftLink&&)=default;

    std::string    path                () const;

};


bool               soft_link_exists    (Group const& group,
                                        std::string const& name);

SoftLink           create_soft_link    (Group& group,
                                        Identifier const& target,
                                        std::string const& name);

} // namespace hdf5
} // namespace lue

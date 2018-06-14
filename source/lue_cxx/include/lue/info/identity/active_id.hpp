#pragma once
#include "lue/array/same_shape/constant_shape/value.hpp"


namespace lue {

class ActiveID:
    public same_shape::constant_shape::Value
{

public:

                   ActiveID            (hdf5::Group const& parent,
                                        std::string const& name);

                   ActiveID            (same_shape::constant_shape::Value&&
                                            value);

                   ActiveID            (ActiveID const&)=delete;

                   ActiveID            (ActiveID&&)=default;

                   ~ActiveID           ()=default;

    ActiveID&      operator=           (ActiveID const&)=delete;

    ActiveID&      operator=           (ActiveID&&)=default;

private:


};


ActiveID           create_active_id    (hdf5::Group& parent);

}  // namespace lue

#pragma once
#include "lue/item/same_shape/constant_shape/continuous_value.hpp"


namespace lue {

class Id:
    public same_shape::constant_shape::ContinuousValue
{

public:

                   Id                  (hdf5::Group& parent);

                   Id                  (Id const&)=delete;

                   Id                  (Id&&)=default;

                   ~Id                 ()=default;

    Id&            operator=           (Id const&)=delete;

    Id&            operator=           (Id&&)=default;

private:

};


Id                 create_id           (hdf5::Group& parent);

}  // namespace lue

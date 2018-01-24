#pragma once
#include "lue/item/constant_collection/constant_shape/same_shape/constant.hpp"


namespace lue {
namespace constant_collection {
namespace stationary {

class Box:
    public constant_shape::same_shape::Constant
{

public:

                   Box                 (Box const&)=delete;

                   Box                 (Box&&)=default;

                   ~Box                ()=default;

    Box&           operator=           (Box const&)=delete;

    Box&           operator=           (Box&&)=default;

private:

};

}  // namespace stationary
}  // namespace constant_collection
}  // namespace lue

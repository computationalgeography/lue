#pragma once
#include "lue/item/constant_collection/constant_shape/same_shape/constant.hpp"


namespace lue {
namespace constant_collection {
namespace stationary {

class Point:
    public constant_shape::same_shape::Constant
{

public:

                   Point               (Point const&)=delete;

                   Point               (Point&&)=default;

                   ~Point              ()=default;

    Point&         operator=           (Point const&)=delete;

    Point&         operator=           (Point&&)=default;

private:

};

}  // namespace lue
}  // namespace constant_collection
}  // namespace stationary

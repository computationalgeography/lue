#pragma once
#include <boost/geometry/index/rtree.hpp>


namespace lue {

template<
    typename Value>
using SpatialIndex = boost::geometry::index::rtree<
    Value,
    boost::geometry::index::quadratic<16>
>;

}  // namespace lue

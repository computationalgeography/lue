#pragma once
#include "lue/hdf5.hpp"
#include <vector>


namespace lue {
namespace data_model {

using Index = hsize_t;

using ID = hsize_t;
using IDs = std::vector<ID>;

using Rank = int;

using Count = hsize_t;
using Counts = std::vector<Count>;

using Shapes = std::vector<hdf5::Shape>;


enum class TimeDomainItemType
{

    point,

    box,

    cell

};


/*!
    @brief      Space domain items can be stationary or mobile through
                time
*/
enum class Mobility
{
    //! Space domain items stay in the same location for the whole time domain
    stationary,

    //! Space domain items move around through time
    mobile
};


enum class SpaceDomainItemType
{

    point,

    box,

    // line,

    // region,

    // cell

};


enum class ShapePerObject
{
    same,
    different
};


enum class ValueVariability
{
    constant,
    variable
};


enum class ShapeVariability
{
    constant,
    variable
};


enum class TimeDiscretization
{
    regular_grid
};


enum class SpaceDiscretization
{
    regular_grid
};

} // namespace data_model
} // namespace lue

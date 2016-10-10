#pragma once
#include "lue/cxx_api/bitmask_operators.hpp"
#include "lue/c_api/define.h"
#include <hdf5.h>


namespace lue {

using item_t = lue_item_t;

using rank_t = uint16_t;

//! Size of collections.
using count_t = uint64_t;

//! Size of collections.
using index_t = count_t;

//! Size of dimension.
using extent_t = hsize_t;


// enum class TypeId
// {
//     uint_32,
//     uint_64,
//     int_32,
//     int_64,
//     float_32,
//     float_64,
// };


enum class TimeDomainTypeAspects: int
{

    omnipresent = 2,
    located = 4,

    shared = 8,
    unique = 16,

    constant_size = 32,
    variable_size = 64

};


enum class SpaceDomainTypeAspects: unsigned int
{

    omnipresent = 1u << 0,
    located = 1u << 1,

    stationary = 1u << 2,
    mobile = 1u << 3,

    indexed = 1u << 4,
    not_indexed = 1u << 5,

    topological = 1u << 6,
    non_topological = 1u << 7

};


// enum class ValueShapeAspect: int
// {
// 
//     //! The shape of values does not change through time
//     constant_through_time = 2,
// 
//     //! The shape of values changes through time.
//     variable_through_time = 4,
// 
//     //! The shape of values is the same for all items.
//     same_per_item = 8,
// 
//     //! The shape of values is different for each item.
//     different_per_item = 16
// 
// };

}  // namespace lue


template<>
struct enable_bitmask_operators<lue::TimeDomainTypeAspects>
{
    static constexpr bool enable = true;
};


template<>
struct enable_bitmask_operators<lue::SpaceDomainTypeAspects>
{
    static constexpr bool enable = true;
};


namespace lue {

enum class TimeDomainType: int
{

    omnipresent = static_cast<int>(
        TimeDomainTypeAspects::omnipresent),

    shared_constant_collection = static_cast<int>(
        TimeDomainTypeAspects::located |
        TimeDomainTypeAspects::shared |
        TimeDomainTypeAspects::constant_size),

    shared_variable_collection = static_cast<int>(
        TimeDomainTypeAspects::located |
        TimeDomainTypeAspects::shared |
        TimeDomainTypeAspects::variable_size),

    unique_constant_collection = static_cast<int>(
        TimeDomainTypeAspects::located |
        TimeDomainTypeAspects::unique |
        TimeDomainTypeAspects::constant_size),

    unique_variable_collection = static_cast<int>(
        TimeDomainTypeAspects::located |
        TimeDomainTypeAspects::unique |
        TimeDomainTypeAspects::variable_size)

};


enum class TimeDomainItemType
{

    none,

    point,

    period,

    cell

};


enum class SpaceDomainType: unsigned int
{

    omnipresent = static_cast<unsigned int>(
        SpaceDomainTypeAspects::omnipresent),

    stationary = static_cast<unsigned int>(
        SpaceDomainTypeAspects::located |
        SpaceDomainTypeAspects::stationary |
        SpaceDomainTypeAspects::not_indexed |
        SpaceDomainTypeAspects::non_topological)

};


enum class SpaceDomainItemType
{

    none,

    point,

    box,

    line,

    region,

    cell

};


enum class TimeDiscretizationType
{

    regular_grid

};


enum class SpaceDiscretizationType
{

    /*!
        @brief  {1,2,3}D Euclidian space.

        Cells have equal sizes in each dimension.
    */
    cartesian_grid,

    // /*!
    //     @brief  {1,2,3}D Euclidian space.

    //     Cells have different sizes in each dimension.
    // */
    // rectilinear_grid,

    // curvilinear_grid,

};

}  // namespace lue

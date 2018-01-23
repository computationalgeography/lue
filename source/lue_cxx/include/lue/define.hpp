#pragma once
#include <hdf5.h>


namespace lue {

// using item_t = lue_item_t;
// 
// /*!
//     @brief      Number of dimensions in an HDF5 dataset
// */
// using rank_t = uint16_t;
// 
// /*!
//     @brief      Size of collections
// */
// using count_t = uint64_t;
// 
// /*!
//     @brief      Size of collections
// */
// using index_t = count_t;
// 
// /*!
//     @brief      Size of dimensions
// */
// using extent_t = hsize_t;


// enum class TypeId
// {
//     uint_32,
//     uint_64,
//     int_32,
//     int_64,
//     float_32,
//     float_64,
// };


// enum class TimeDomainTypeAspects: int
// {
// 
//     omnipresent = 2,
//     located = 4,
// 
//     shared = 8,
//     unique = 16,
// 
//     // constant_size = 32,
//     // variable_size = 64
// 
// };


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


// template<>
// struct enable_bitmask_operators<lue::TimeDomainTypeAspects>
// {
//     static constexpr bool enable = true;
// };


// template<>
// struct enable_bitmask_operators<lue::SpaceDomainTypeAspects>
// {
//     static constexpr bool enable = true;
// };


namespace lue {

/*!
    @brief      The size of the item collection can be constant or variable
                through time
    @warning    The size of the item collection cannot be variable in
                case the time domain type is omnipresent.
*/
enum class SizeOfItemCollectionType
{

    //! The collection of items remains constant through time
    constant_size,

    // //! The collection of items is variable through time
    // variable_size

};


// enum class TimeDomainType  // : int
// {
// 
//     omnipresent,
// 
//     shared,
// 
//     // unique
// 
//     // omnipresent = static_cast<int>(
//     //     TimeDomainTypeAspects::omnipresent),
// 
//     // shared_constant_collection = static_cast<int>(
//     //     TimeDomainTypeAspects::located |
//     //     TimeDomainTypeAspects::shared) //  |
//     //     // TimeDomainTypeAspects::constant_size),
// 
//     // shared_variable_collection = static_cast<int>(
//     //     TimeDomainTypeAspects::located |
//     //     TimeDomainTypeAspects::shared |
//     //     TimeDomainTypeAspects::variable_size),
// 
//     // unique_constant_collection = static_cast<int>(
//     //     TimeDomainTypeAspects::located |
//     //     TimeDomainTypeAspects::unique |
//     //     TimeDomainTypeAspects::constant_size),
// 
//     // unique_variable_collection = static_cast<int>(
//     //     TimeDomainTypeAspects::located |
//     //     TimeDomainTypeAspects::unique |
//     //     TimeDomainTypeAspects::variable_size)
// 
// };


// enum class TimeDomainItemType
// {
// 
//     none,
// 
//     // point,
// 
//     period,
// 
//     // cell
// 
// };
// 
// 
// enum class SpaceDomainType
// {
// 
//     omnipresent,
// 
//     located
// 
// };
// 
// 
// /*!
//     @brief      Space domain items can be stationary or mobile through time
// 
//     This is only relevant in case the time domain is not omnipresent.
// */
// enum class Mobility
// {
//     //! Space domain items stay in the same location for the whole time domain
//     stationary,
// 
//     //! Space domain items move around through time
//     mobile
// };
// 
// 
// enum class SpaceDomainItemType
// {
// 
//     none,
// 
//     // point,
// 
//     box,
// 
//     // line,
// 
//     // region,
// 
//     // cell
// 
// };
// 
// 
// enum class TimeDiscretizationType
// {
// 
//     regular_grid
// 
// };
// 
// 
// enum class SpaceDiscretizationType
// {
// 
//     /*!
//         @brief  {1,2,3}D Euclidian space.
// 
//         Cells have equal sizes in each dimension.
//     */
//     cartesian_grid,
// 
//     // /*!
//     //     @brief  {1,2,3}D Euclidian space.
// 
//     //     Cells have different sizes in each dimension.
//     // */
//     // rectilinear_grid,
// 
//     // curvilinear_grid,
// 
// };
// 
// 
// enum class TimeResolution
// {
// 
//     // millennium,
//     // century,
//     // decade,
//     // year,
//     month,
//     // week,
//     // day,
//     // hour,
//     // minute,
//     // second,
//     // decisecond,  // 1/10s
//     // centisecond,  // 1/100s
//     // millisecond,  // 1/1000s
// 
// };


// /*!
//     @brief      The shape of the value of different items can be the
//                 same or different
// 
//     Whether or not the shape of the value of different items is the same
//     or different is independent of the time domain type.
// */
// enum class ShapePerItemType
// {
// 
//     //! The value of all items have the same shape
//     same,
// 
//     //! The value of all items have a different shape (potentially)
//     different
// 
// };


// enum class ShapeThroughTime
// {
// 
//     //! The shape of each item's value does not change through time
//     constant,
// 
//     //! The shape of each item's value changes through time
//     variable
// 
// };

}  // namespace lue

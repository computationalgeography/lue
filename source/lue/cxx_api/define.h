#pragma once
#include "lue/cxx_api/bitmask_operators.hpp"


namespace lue {

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


enum class SpaceDomainTypeAspects: int
{

    omnipresent = 2,
    located = 4,

    indexed = 8,
    not_indexed = 16,

    topological = 32,
    non_topological = 64

};

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


enum class SpaceDomainType: int
{

    omnipresent = static_cast<int>(
        SpaceDomainTypeAspects::omnipresent),

    located = static_cast<int>(
        SpaceDomainTypeAspects::located |
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

}  // namespace lue

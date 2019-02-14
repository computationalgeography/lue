#pragma once
#include <array>
#include <numeric>


namespace lue {

template<
    typename Index,
    std::size_t rank>
using Shape = std::array<Index, rank>;


// template<
//     typename Index,
//     std::size_t rank>
// class Shape
// {
// 
// public:
// 
//     // using std::array<Index, rank>::array;
// 
//     Shape()
//         _array{}
//     {
// 
//     }
// 
// private:
// 
//     std::array<Index, rank> _array;
// 
// };


template<
    typename Index,
    std::size_t rank>
inline typename Shape<Index, rank>::value_type nr_elements(
    Shape<Index, rank> const& shape)
{
    return std::accumulate(
        shape.begin(), shape.end(), 1,
        std::multiplies<typename Shape<Index, rank>::value_type>());
}

}  // namespace lue

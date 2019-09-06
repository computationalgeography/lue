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
    static_assert(!shape.empty());

    using T = typename Shape<Index, rank>::value_type;

    return std::accumulate(
        shape.begin(), shape.end(), T{1},
        std::multiplies<T>());
}


template<
    typename Index>
inline constexpr typename Shape<Index, 0>::value_type nr_elements(
    Shape<Index, 0> const& /* shape */)
{
    return 0;
}

}  // namespace lue

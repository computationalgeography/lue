#pragma once
#include <cassert>
#include <numeric>


namespace lue {
namespace test {

template<
    typename Collection1,
    typename Collection2,
    typename Rank=std::size_t>
inline void generate_values(
    Collection1& values,
    Collection2 const& counts,
    Rank const rank=1)
{
    assert(values.size() == counts.size());

    for(std::size_t i = 0; i < counts.size(); ++i) {
        values[i].resize(counts[i] * rank);
        std::iota(values[i].begin(), values[i].end(), i);
    }
}

}  // namespace test
}  // namespace lue

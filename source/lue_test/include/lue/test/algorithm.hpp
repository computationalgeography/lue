#pragma once
#include <boost/range/combine.hpp>
#include <cassert>
#include <numeric>
#include <random>
#include <type_traits>


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


template<
    typename Collection>
inline void generate_random_ids(
    Collection& ids)
{
    // Fill collection with range of unique integers and shuffle the
    // collection

    std::iota(ids.begin(), ids.end(), 0);

    // Shuffle the collection
    // The device provides a seed
    // The engine provides random numbers
    std::random_device random_device;
    std::mt19937 random_number_engine{random_device()};
    std::shuffle(ids.begin(), ids.end(), random_number_engine);
}


template<
    typename Collections,
    typename Counts>
inline void select_random_ids(
    Collections& id_collections,
    Counts const& id_counts)
{
    assert(id_collections.size() == id_counts.size());
    assert(!id_counts.empty());

    // Create a collection of unique IDs with the size of the maximum
    // number of IDs
    auto max_count = *std::max_element(id_counts.begin(), id_counts.end());

    using Collection = typename Collections::value_type;
    Collection all_ids(max_count);
    generate_random_ids(all_ids);

    std::random_device random_device;
    std::mt19937 random_number_engine{random_device()};
    std::uniform_int_distribution<typename Collection::size_type>
        distribution{0, max_count - 1};

    // For each collection, randomly select IDs
    // for(auto const& [id_collection, id_count]: boost::combine(id_collections, id_counts)) {
    for(std::size_t i = 0; i < id_collections.size(); ++i) {
        auto& id_collection = id_collections[i];
        auto& id_count = id_counts[i];

        id_collection.resize(id_count);
        std::generate(id_collection.begin(), id_collection.end(),
            [&all_ids, &random_number_engine, &distribution](){
                return all_ids[distribution(random_number_engine)];
            }
        );
    }
}


template<
    typename Collection>
inline void generate_random_integral_values(
    Collection& values,
    typename Collection::value_type const min,
    typename Collection::value_type const max)
{
    using ValueType = typename Collection::value_type;

    static_assert(std::is_integral<ValueType>(), "");

    // Fill collection with range of random integers within [min, max].
    // Some of the values might occur more than once.

    // The device provides a seed
    // The engine provides random numbers
    std::random_device random_device;
    std::mt19937 random_number_engine{random_device()};
    std::uniform_int_distribution<ValueType>
        distribution{min, max};
    std::generate(values.begin(), values.end(),
        [&distribution, &random_number_engine]() {
            return distribution(random_number_engine);
        }
    );
}


template<
    typename Collection>
inline void generate_random_real_values(
    Collection& values,
    typename Collection::value_type const min,
    typename Collection::value_type const max)
{
    using ValueType = typename Collection::value_type;

    static_assert(std::is_floating_point<ValueType>(), "");

    // Fill collection with range of random reals within [min, max].
    // Some of the values might occur more than once (unlikely).

    // The device provides a seed
    // The engine provides random numbers
    std::random_device random_device;
    std::mt19937 random_number_engine{random_device()};
    std::uniform_real_distribution<ValueType>
        distribution{min, max};
    std::generate(values.begin(), values.end(),
        [&distribution, &random_number_engine]() {
            return distribution(random_number_engine);
        }
    );
}


template<
    typename Collection>
inline void generate_random_counts(
    Collection& values,
    typename Collection::value_type const min,
    typename Collection::value_type const max)
{
    generate_random_integral_values(values, min, max);
}


// template<
//     typename Collection,
//     typename = std::enable_if_t<std::is_integral<typename Collection::value_type>::value>>
// inline void generate_random_values(
//     Collection& values,
//     typename Collection::value_type const min,
//     typename Collection::value_type const max)
// {
//     generate_random_integral_values(values, min, max);
// }


template<
    typename Collection,
    typename = std::enable_if_t<std::is_floating_point<typename Collection::value_type>::value>>
inline void generate_random_values(
    Collection& values,
    typename Collection::value_type const min,
    typename Collection::value_type const max)
{
    generate_random_real_values(values, min, max);
}


// C++17:
// template<
//     typename Collection>
// inline void generate_random_values(
//     Collection& values,
//     typename Collection::value_type const min,
//     typename Collection::value_type const max)
// {
//     using ValueType = typename Collection::value_type;
// 
//     if constexpr(std::is_integral<ValueType>()) {
//         generate_random_integral_values(values, min, max);
//     }
//     else {
//         generate_random_real_values(values, min, max);
//     }
// }

}  // namespace test
}  // namespace lue

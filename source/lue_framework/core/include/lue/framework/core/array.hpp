#pragma once


namespace lue {

// Default array funtion templates
template<
    typename Array>
std::size_t rank(Array const& array);


template<
    typename Array,
    typename Shape>
Shape shape_in_partitions(Array const& array);


template<
    typename Array>
std::size_t nr_partitions(Array const& array);

}  // namespace lue

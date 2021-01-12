#pragma once
#include <limits>


namespace lue {

template<
    typename FlowDirection>
constexpr FlowDirection north_west{7};

template<
    typename FlowDirection>
constexpr FlowDirection north{8};

template<
    typename FlowDirection>
constexpr FlowDirection north_east{9};

template<
    typename FlowDirection>
constexpr FlowDirection west{4};

template<
    typename FlowDirection>
constexpr FlowDirection sink{5};

template<
    typename FlowDirection>
constexpr FlowDirection east{6};

template<
    typename FlowDirection>
constexpr FlowDirection south_west{1};

template<
    typename FlowDirection>
constexpr FlowDirection south{2};

template<
    typename FlowDirection>
constexpr FlowDirection south_east{3};

template<
    typename FlowDirection>
constexpr FlowDirection no_data{std::numeric_limits<FlowDirection>::max()};

}  // namespace lue

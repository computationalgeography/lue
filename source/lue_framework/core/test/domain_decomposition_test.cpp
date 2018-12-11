#define BOOST_TEST_MODULE lue framework core domain_decomposition
#include <boost/test/unit_test.hpp>
#include "lue/framework/core/domain_decomposition.hpp"
#include <iostream>


// template<
//     typename T,
//     std::size_t rank>
// std::ostream& operator<<(
//     std::ostream& stream,
//     std::array<T, rank> const& array)
// {
//     stream << "[";
// 
//     if(rank > 0) {
//         for(std::size_t i = 0; i < rank - 1; ++i) {
//             stream << array[i] << ", ";
//         }
//         stream << array.back();
//     }
// 
//     stream << "]";
//     return stream;
// }


BOOST_AUTO_TEST_CASE(nr_grains)
{
    using T = std::uint64_t;
    std::size_t const rank = 2;
    using Shape = lue::Shape<T, rank>;

    Shape area_shape{6, 4};

    {
        Shape grain_shape{1, 1};
        auto nr_grains = lue::nr_grains(area_shape, grain_shape);
        BOOST_CHECK_EQUAL(nr_grains, 24);
    }

    {
        Shape grain_shape{2, 2};
        auto nr_grains = lue::nr_grains(area_shape, grain_shape);
        BOOST_CHECK_EQUAL(nr_grains, 6);
    }

    {
        Shape grain_shape{3, 3};
        auto nr_grains = lue::nr_grains(area_shape, grain_shape);
        BOOST_CHECK_EQUAL(nr_grains, 4);
    }

    {
        Shape grain_shape{4, 4};
        auto nr_grains = lue::nr_grains(area_shape, grain_shape);
        BOOST_CHECK_EQUAL(nr_grains, 2);
    }

    {
        Shape grain_shape{5, 5};
        auto nr_grains = lue::nr_grains(area_shape, grain_shape);
        BOOST_CHECK_EQUAL(nr_grains, 2);
    }

    {
        Shape grain_shape{10, 10};
        auto nr_grains = lue::nr_grains(area_shape, grain_shape);
        BOOST_CHECK_EQUAL(nr_grains, 1);
    }
}


BOOST_AUTO_TEST_CASE(linear_to_shape_index)
{
    using T = std::uint64_t;
    std::size_t const rank = 2;
    using Shape = lue::Shape<T, rank>;
    using Indices = lue::Indices<T, rank>;

    Shape area_shape{6, 4};
    T index;

    {
        index = 0;
        auto indices = lue::linear_to_shape_index(area_shape, index);
        BOOST_CHECK(indices == (Indices{0, 0}));
    }

    {
        index = 3;
        auto indices = lue::linear_to_shape_index(area_shape, index);
        BOOST_CHECK(indices == (Indices{0, 3}));
    }

    {
        index = 4;
        auto indices = lue::linear_to_shape_index(area_shape, index);
        BOOST_CHECK(indices == (Indices{1, 0}));
    }


}

#define BOOST_TEST_MODULE lue framework core index_util
#include <hpx/config.hpp>
#include <boost/test/unit_test.hpp>
#include "lue/framework/core/index_util.hpp"


BOOST_AUTO_TEST_CASE(d2)
{
    lue::Rank const rank{2};
    using Shape = lue::Shape<lue::Count, rank>;
    using Indices = lue::Indices<lue::Index, rank>;

    {
        Shape const shape{5, 6};
        lue::Index const idx{20};

        Indices idxs = lue::linear_idx_to_idxs(idx, shape);
        BOOST_CHECK_EQUAL(idxs[0], 3);
        BOOST_CHECK_EQUAL(idxs[1], 2);
        BOOST_CHECK_EQUAL(lue::idxs_to_linear_idx(idxs, shape), idx);
    }

    {
        Shape const shape{1, 1};
        lue::Index const idx{0};

        Indices idxs = lue::linear_idx_to_idxs(idx, shape);
        BOOST_CHECK_EQUAL(idxs[0], 0);
        BOOST_CHECK_EQUAL(idxs[1], 0);
        BOOST_CHECK_EQUAL(lue::idxs_to_linear_idx(idxs, shape), idx);
    }

    {
        Shape const shape{5, 6};
        lue::Index const idx{0};

        Indices idxs = lue::linear_idx_to_idxs(idx, shape);
        BOOST_CHECK_EQUAL(idxs[0], 0);
        BOOST_CHECK_EQUAL(idxs[1], 0);
        BOOST_CHECK_EQUAL(lue::idxs_to_linear_idx(idxs, shape), idx);
    }

    {
        Shape const shape{5, 6};
        lue::Index const idx{29};

        Indices idxs = lue::linear_idx_to_idxs(idx, shape);
        BOOST_CHECK_EQUAL(idxs[0], 4);
        BOOST_CHECK_EQUAL(idxs[1], 5);
        BOOST_CHECK_EQUAL(lue::idxs_to_linear_idx(idxs, shape), idx);
    }
}

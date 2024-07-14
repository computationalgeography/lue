#define BOOST_TEST_MODULE lue gdal blocks_test
#include "lue/gdal/blocks.hpp"
#include "lue/stream.hpp"
#include <boost/test/included/unit_test.hpp>


BOOST_AUTO_TEST_CASE(constructor)
{
    namespace lgd = lue::gdal;

    {
        lgd::Shape const raster_shape{60, 40};
        lgd::Shape const block_shape{6, 4};
        lgd::Blocks const blocks{lgd::Blocks{raster_shape, block_shape}};

        BOOST_CHECK_EQUAL(blocks.raster_shape(), raster_shape);
        BOOST_CHECK_EQUAL(blocks.block_shape(), block_shape);
        BOOST_CHECK_EQUAL(blocks.shape_in_blocks(), (lgd::Shape{10, 10}));
    }

    {
        lgd::Shape const raster_shape{60, 40};
        lgd::Shape const block_shape{60, 40};
        lgd::Blocks const blocks{lgd::Blocks{raster_shape, block_shape}};


        BOOST_CHECK_EQUAL(blocks.raster_shape(), raster_shape);
        BOOST_CHECK_EQUAL(blocks.block_shape(), block_shape);
        BOOST_CHECK_EQUAL(blocks.shape_in_blocks(), (lgd::Shape{1, 1}));
    }

    {
        lgd::Shape const raster_shape{60, 40};
        lgd::Shape const block_shape{1, 1};
        lgd::Blocks const blocks{lgd::Blocks{raster_shape, block_shape}};

        BOOST_CHECK_EQUAL(blocks.raster_shape(), raster_shape);
        BOOST_CHECK_EQUAL(blocks.block_shape(), block_shape);
        BOOST_CHECK_EQUAL(blocks.shape_in_blocks(), (lgd::Shape{60, 40}));
    }

    {
        lgd::Shape const raster_shape{50, 30};
        lgd::Shape const block_shape{6, 4};
        lgd::Blocks const blocks{lgd::Blocks{raster_shape, block_shape}};

        BOOST_CHECK_EQUAL(blocks.raster_shape(), raster_shape);
        BOOST_CHECK_EQUAL(blocks.block_shape(), block_shape);
        BOOST_CHECK_EQUAL(blocks.shape_in_blocks(), (lgd::Shape{9, 8}));
    }
}


BOOST_AUTO_TEST_CASE(shape_in_valid_cells)
{
    namespace lgd = lue::gdal;

    {
        lgd::Shape const raster_shape{50, 30};
        lgd::Shape const block_shape{6, 4};
        lgd::Blocks const blocks{lgd::Blocks{raster_shape, block_shape}};

        BOOST_CHECK_EQUAL(blocks.shape_in_valid_cells(lgd::Offset{0, 0}), block_shape);
        BOOST_CHECK_EQUAL(blocks.shape_in_valid_cells(lgd::Offset{8, 0}), (lgd::Shape{2, 4}));
        BOOST_CHECK_EQUAL(blocks.shape_in_valid_cells(lgd::Offset{0, 7}), (lgd::Shape{6, 2}));
        BOOST_CHECK_EQUAL(blocks.shape_in_valid_cells(lgd::Offset{8, 7}), (lgd::Shape{2, 2}));
    }
}

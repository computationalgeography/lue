#define BOOST_TEST_MODULE lue framework partitioned_raster partitioned_raster
#include "lue/framework/core/component.hpp"
#include "lue/framework/partitioned_raster.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"


namespace {

    using Element = std::int32_t;
    using PartitionedRaster = lue::PartitionedRaster<Element>;

    using MBR = typename PartitionedRaster::MBR;
    using Cells = typename PartitionedRaster::Cells;

    using Data = lue::DataT<PartitionedRaster>;

    // using PartitionClient = lue::client::Partition<Index, Element, rank>;
    // using PartitionServer = typename PartitionClient::Server;
    // using Data = typename PartitionClient::Data;

    // using Definition = typename PartitionedRaster::Definition;
    using Count = typename Data::Count;
    // using Offset = typename PartitionedRaster::Offset;
    using Shape = typename PartitionedRaster::Shape;
    using CellShape = typename PartitionedRaster::CellShape;

}  // Anonymous namespace


BOOST_AUTO_TEST_CASE(default_construct)
{
    // Default initialization
    {
        PartitionedRaster const raster;

        BOOST_CHECK_EQUAL(raster.crs(), lue::Cartesian2D);

        MBR const mbr{{{{0.0, 0.0}, {0.0, 0.0}}}};
        BOOST_CHECK_EQUAL(raster.mbr(), mbr);

        BOOST_CHECK_EQUAL(raster.nr_cells(), 0);

        Shape const shape{{0, 0}};
        BOOST_CHECK_EQUAL(raster.shape(), shape);

        CellShape const cell_shape{{0, 0}};
        BOOST_CHECK_EQUAL(raster.cell_shape(), cell_shape);
    }

    // Value initialization
    {
        PartitionedRaster const raster{};

        BOOST_CHECK_EQUAL(raster.crs(), lue::Cartesian2D);

        MBR const mbr{{{{0.0, 0.0}, {0.0, 0.0}}}};
        BOOST_CHECK_EQUAL(raster.mbr(), mbr);

        BOOST_CHECK_EQUAL(raster.nr_cells(), 0);

        Shape const shape{{0, 0}};
        BOOST_CHECK_EQUAL(raster.shape(), shape);

        CellShape const cell_shape{{0, 0}};
        BOOST_CHECK_EQUAL(raster.cell_shape(), cell_shape);
    }
}


BOOST_AUTO_TEST_CASE(construct_from_cells)
{
    Count const nr_rows = 300;
    Count const nr_cols = 400;
    Shape const shape{{nr_rows, nr_cols}};
    Element const fill_value{5};

    Cells cells = lue::create_partitioned_array(shape, fill_value);

    PartitionedRaster const raster{std::move(cells)};

    BOOST_CHECK_EQUAL(raster.crs(), lue::Cartesian2D);

    MBR const mbr{{{{0.0, 400.0}, {0.0, 300.0}}}};
    BOOST_CHECK_EQUAL(raster.mbr(), mbr);

    BOOST_CHECK_EQUAL(raster.nr_cells(), nr_rows * nr_cols);

    BOOST_CHECK_EQUAL(raster.shape(), shape);

    BOOST_CHECK_EQUAL(raster.mbr().extent(0), 400);
    BOOST_CHECK_EQUAL(raster.mbr().extent(1), 300);

    CellShape const cell_shape{{1, 1}};
    BOOST_CHECK_EQUAL(raster.cell_shape(), cell_shape);
}

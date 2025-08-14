#pragma once
#include "lue/framework/core/bounding_box.hpp"
#include "lue/framework/partitioned_array.hpp"
#include "lue/framework/partitioned_raster/crs.hpp"


namespace lue {

    /*!
        @brief      Class for representing partitioned rasters
        @tparam     Element Type for representing element values

        The raster is partitioned. Partitions can be located in multiple processes.
    */
    template<typename Element>
    class PartitionedRaster
    {

        public:

            //! Type for representing the raster cells
            using Cells = PartitionedArray<Element, 2>;

            //! Type for representing a raster's shape, in (nr_rows, nr_cols)
            using Shape = typename Cells::Shape;

            //! Type for representing coordinates
            using Coordinate = double;

            //! Type for representing a cell's shape, in (width, height)
            using CellShape = std::array<Coordinate, 2>;

            //! Type for representing a raster's minimum bounding rectangle
            using MBR = BoundingBox<Coordinate, 2>;

            /*!
                @brief      Default-construct an instance with an empty shape

                The raster will have zero cells. The crs will be set to EPSG:1024 (Cartesian 2D CS).
            */
            PartitionedRaster() = default;


            /*!
                @brief      Construct an instance given a collection of raster cells

                The mbr will be set to (0, nr_cols) - (0, nr_rows).
                The crs will be set to EPSG:1024 (Cartesian 2D CS).

                This constructor makes it easy to convert from a partitioned array representing a spatial
                field in a Cartesian plan to a partitioned raster.
            */
            PartitionedRaster(Cells&& cells):

                _mbr{MBR{{{{0.0, double(cells.shape()[1])}, {0.0, double(cells.shape()[0])}}}}},
                _cells{std::move(cells)}

            {
            }


            /*!
                @brief      Construct an instance given a collection of raster cells and information about
                            the coordinate reference system
            */
            PartitionedRaster(CRS const& crs, MBR const& mbr, Cells&& cells):

                _crs{crs},
                _mbr{mbr},
                _cells{std::move(cells)}

            {
            }


            PartitionedRaster(PartitionedRaster const& other) = delete;

            PartitionedRaster(PartitionedRaster&& other) noexcept = default;

            ~PartitionedRaster() = default;

            auto operator=(PartitionedRaster const& other) -> PartitionedRaster& = delete;

            auto operator=(PartitionedRaster&& other) noexcept -> PartitionedRaster& = default;


            /*!
                @brief      Return the coordinate reference system
            */
            [[nodiscard]] auto crs() const -> CRS const&
            {
                return _crs;
            }


            /*!
                @brief      Return the minimum bounding rectangle
            */
            auto mbr() const -> MBR const&
            {
                return _mbr;
            }


            /*!
                @brief      Return the number of cells
            */
            [[nodiscard]] auto nr_cells() const -> Count
            {
                return _cells.nr_elements();
            }


            /*!
                @brief      Return the shape as (nr rows, nr cols)
            */
            [[nodiscard]] auto shape() const -> Shape const&
            {
                return _cells.shape();
            }


            /*!
                @brief      Return the shape of each raster cell, in (width, height)
            */
            [[nodiscard]] auto cell_shape() const -> CellShape
            {
                // Compute the shape on the fly, given the shape of the array and the bounding box
                auto const [nr_rows, nr_cols] = this->shape();

                return {
                    nr_cols > 0 ? _mbr.extent(0) / nr_cols : 0,
                    nr_rows > 0 ? _mbr.extent(1) / nr_rows : 0,
                };
            }


            auto cells() const -> Cells const&
            {
                return _cells;
            }


        private:

            // TODO: Figure out how CRSs are represented and use a variant to store these kinds(?). If we
            // need to do transformation / conversion then we can call into the logic that does that.
            // Until then it may be enough to be able to answer these simple questions:

            // - What is the type of CRS? Geographic (lat/lon), geodetic (lat/lon), Cartesian (x, y), ...
            // - What is the cell size?

            // Some operations need this information (slope, etc)
            // EPSG code is very popular. String (WKT) and JSON (PROJJSON) are likely needed as well, also
            // to allow for custom CRSs
            // Given a EPSG code, proj is needed to answer these questions. An alternative approach is to
            // only support PROJJSON format and be able to query that.

            //! The coordinate reference system
            CRS _crs{Cartesian2D};

            //! The minimum bounding rectangle
            MBR _mbr;

            //! The collection of raster cells
            Cells _cells{};
    };


    namespace detail {

        template<typename E>
        class ArrayTraits<PartitionedRaster<E>>
        {

            public:

                using Element = E;

                static constexpr Rank rank = 2;

                // static constexpr bool is_partitioned_array{true};

                using Offset = typename PartitionedArray<E, 2>::Offset;

                using Shape = typename PartitionedRaster<E>::Shape;

                template<typename E_>
                using Partition = typename PartitionedArray<E, 2>::Partition;

                template<typename E_>
                using Partitions = typename PartitionedArray<E_, 2>::Partitions;

                // template<typename E_>
                // using PartitionedRaster = PartitionedRaster<E_>;

                template<typename E_, Rank r>
                using Data = typename Partition<E_>::Data;

                // template<typename E_>
                // using Component = typename PartitionedRaster<E_>::Partition;
                //
                // template<typename E_>
                // using Components = typename PartitionedRaster<E_>::Partitions;
        };

    }  // namespace detail

}  // namespace lue

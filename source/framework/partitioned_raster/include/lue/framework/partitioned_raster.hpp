#pragma once
#include "lue/framework/core/bounding_box.hpp"
#include "lue/framework/partitioned_array.hpp"


namespace lue {

    //! Type for representing European Petroleum Survey Group codes
    enum class EPSGCode : std::int16_t {};

    static EPSGCode const Cartesian2D{1024};

    //! Type for representing Spatial Reference System IDs
    using SRID = EPSGCode;

    //! Type for representing Spatial Reference systems
    using SRS = SRID;


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


            /*!
                @brief      Default-construct an instance with an empty shape

                The raster will have zero cells. The SRS will be set to EPSG:1024 (Cartesian 2D CS).
            */
            PartitionedRaster() = default;


            /*!
                @brief      Construct an instance
                @param      shape Shape of the raster in (nr_rows, nr_cols)
                @param      box Minimum bounding rectangle around the cells
                @param      srs Spatial reference system
            */
            PartitionedRaster(Shape const& shape, BoundingBox<Element, 2> const& box, SRS const& srs):

                _cells{shape},
                _box{box},
                _srs{srs}

            {
            }


            PartitionedRaster(PartitionedRaster const& other) = delete;

            PartitionedRaster(PartitionedRaster&& other) noexcept = default;

            ~PartitionedRaster() = default;

            auto operator=(PartitionedRaster const& other) -> PartitionedRaster& = delete;

            auto operator=(PartitionedRaster&& other) noexcept -> PartitionedRaster& = default;


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
                    nr_rows > 0 ? _box.extent(0) / nr_cols : 0,
                    nr_cols > 1 ? _box.extent(1) / nr_rows : 0,
                };
            }


            /*!
                @brief      Return the spatial reference system
            */
            [[nodiscard]] auto srs() const -> SRS
            {
                return _srs;
            }


        private:

            //! The collection of raster cells
            Cells _cells{};

            //! Minimum bounding rectangle around the cells
            BoundingBox<Coordinate, 2> _box;

            //! The spatial reference system
            SRS _srs{Cartesian2D};
    };


    namespace detail {

    }  // namespace detail
}  // namespace lue

#pragma once
#include "lue/framework/core/define.hpp"
#include <array>
#include <utility>


namespace lue {

    /*!
        @brief      Class template for representing multi-dimensional bounding boxes
        @tparam     Coordinate Type to use for coordinates along the dimensions
        @tparam     rank Number of dimensions

        In a multi-dimensional bounding box, coordinate pairs representing the minimum and maximum limits
        along each dimension are stored.

        For a 1D bounding box, one coordinate pair is needed, for a 2D bounding box 2 coordinate pairs, etc.
        In the general case, @a rank coordinate pairs are needed.
    */
    template<typename Coordinate, Rank rank>
    class BoundingBox
    {
        public:

            //! Pair of minimum and maximum coordinates, associated with a single dimension
            using Limit = std::pair<Coordinate, Coordinate>;

            //! For each dimension a limit
            using Limits = std::array<Limit, rank>;

            //! Type for indexing into a Limits collection
            using Index = typename Limits::size_type;

            //! Type of extent between two coordinates defining a limit
            using Extent = Coordinate;


            BoundingBox() = default;


            /*!
                @brief      Construct an instance, given limits
            */
            BoundingBox(Limits&& limits):

                _limits{std::forward<Limits>(limits)}

            {
            }


            BoundingBox(BoundingBox const&) = default;

            BoundingBox(BoundingBox&&) noexcept = default;

            ~BoundingBox() = default;

            auto operator=(BoundingBox const&) -> BoundingBox& = default;

            auto operator=(BoundingBox&&) noexcept -> BoundingBox& = default;


            /*!
                @brief      Return the minimum limit defined for a dimension
            */
            [[nodiscard]] auto min_limit(Index const dimension_idx) const -> Coordinate
            {
                return std::get<0>(_limits[dimension_idx]);
            }


            /*!
                @brief      Return the maximum limit defined for a dimension
            */
            [[nodiscard]] auto max_limit(Index const dimension_idx) const -> Coordinate
            {
                return std::get<1>(_limits[dimension_idx]);
            }


            /*!
                @brief      Return the extent between the limits defined for a dimension
            */
            auto extent(Index const dimension_idx) const -> Extent
            {
                return this->max_limit(dimension_idx) - this->min_limit(dimension_idx);
            }

        private:

            //! Collection of limits per dimension
            Limits _limits;
    };

}  // namespace lue

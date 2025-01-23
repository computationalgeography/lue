#pragma once
#include "lue/framework/core/offset.hpp"
#include <algorithm>
#include <cassert>
#include <stdexcept>


namespace lue {

    template<Rank rank>
    class Hyperslab
    {

        private:

            using Array = std::array<Index, rank>;

        public:

            using Offsets = Array;
            using Counts = Array;
            using Strides = Array;
            using Center = Array;
            using Shape = Array;


            Hyperslab(Center const& center, Shape const& shape):

                _counts{shape}

            {
                Shape half_shape{shape};

                std::transform(
                    half_shape.begin(),
                    half_shape.end(),
                    half_shape.begin(),
                    [](auto const count) { return 0.5 * (count % 2 == 1 ? count - 1 : count); });

                // Equality in the sense that shape can be centered and not extend into areas with smaller
                // than zero idxs
                if (!std::equal(
                        half_shape.begin(),
                        half_shape.end(),
                        center.begin(),
                        [](auto const count, auto const idx)
                        {
                            // Subtracting this many cells from the center cell results in a negative idx...
                            return idx >= count;
                        }))
                {
                    throw std::runtime_error("Hyperslab shape extents beyond the limits of the array");
                }

                std::transform(
                    center.begin(),
                    center.end(),
                    half_shape.begin(),
                    _offsets.begin(),
                    [](auto const idx, auto const count)
                    {
                        assert(idx >= count);
                        return idx - count;
                    });
                _strides.fill(1);
            }

            auto offsets() const -> Offsets const&
            {
                return _offsets;
            }

            auto counts() const -> Counts const&
            {
                return _counts;
            }

            auto strides() const -> Strides const&
            {
                return _strides;
            }

            [[nodiscard]] auto is_strided() const -> bool
            {
                return std::ranges::any_of(_strides, [](auto const stride) { return stride > 1; });
            }

            // Returns whether the indices represented by the hyperslab all fall within the bounds of the
            // array whose shape is passed in. By definition (see constructor), the minimum indices of the
            // hyperslab are fine, so only the maximum indices are checked here.
            auto is_contained_by(Shape const& array_shape) const -> bool
            {
                for (std::size_t idx = 0; idx < rank; ++idx)
                {
                    if (!(_offsets[idx] + _counts[idx] - 1 < array_shape[idx]))
                    {
                        return false;
                    }
                }

                return true;
            }

        private:

            Offsets _offsets{};

            Counts _counts;

            Strides _strides{};
    };

}  // namespace lue

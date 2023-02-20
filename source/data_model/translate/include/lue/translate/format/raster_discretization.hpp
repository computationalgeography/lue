#pragma once
#include <array>
#include <hdf5.h>


namespace lue {
    namespace hl {

        class RasterDiscretization
        {

            public:

                // [nr_rows, nr_cols]
                using Shape = std::array<hsize_t, 2>;

                RasterDiscretization() = default;

                RasterDiscretization(hsize_t nr_rows, hsize_t nr_cols);

                explicit RasterDiscretization(Shape&& shape);

                RasterDiscretization(RasterDiscretization const&) = default;

                RasterDiscretization(RasterDiscretization&&) = default;

                ~RasterDiscretization() = default;

                RasterDiscretization& operator=(RasterDiscretization const&) = default;

                RasterDiscretization& operator=(RasterDiscretization&&) = default;

                bool operator==(RasterDiscretization const& other) const;

                bool operator<(RasterDiscretization const& other) const;

                Shape const& shape() const;

                hsize_t nr_rows() const;

                hsize_t nr_cols() const;

            private:

                Shape _shape;
        };

    }  // namespace hl
}  // namespace lue

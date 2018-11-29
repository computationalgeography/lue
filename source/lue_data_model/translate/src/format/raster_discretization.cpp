#include "lue/translate/format/raster_discretization.hpp"


namespace lue {
namespace hl {

RasterDiscretization::RasterDiscretization(
    Shape&& shape)

    : _shape(std::forward<Shape>(shape))

{
}


RasterDiscretization::RasterDiscretization(
    hsize_t const nr_rows,
    hsize_t const nr_cols)

    : _shape{{nr_rows, nr_cols}}

{
}


RasterDiscretization::Shape const& RasterDiscretization::shape() const
{
    return _shape;
}


hsize_t RasterDiscretization::nr_rows() const
{
    return _shape[0];
}


hsize_t RasterDiscretization::nr_cols() const
{
    return _shape[1];
}


bool RasterDiscretization::operator==(
    RasterDiscretization const& other) const
{
    return _shape == other._shape;
}


bool RasterDiscretization::operator<(
    RasterDiscretization const& other) const
{
    return _shape < other._shape;
}

}  // namespace hl
}  // namespace lue

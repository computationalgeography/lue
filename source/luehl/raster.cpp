#include "luehl/raster.h"
#include "lue/constant_size/time/omnipresent/different_shape/property.h"
#include "lue/constant_size/time/omnipresent/space_box_domain.h"
#include "lue/hdf5/datatype_traits.h"
#include <iostream>


namespace lue {
namespace hl {
namespace {

omnipresent::same_shape::Property discretization_property(
    omnipresent::PropertySet& property_set,
    Raster::Discretization const& discretization)
{
    size_t const nr_items = 1;

    // nr_items x 1D value.
    size_t const rank = 2;

    // Two values per item: nr_rows, nr_cols.
    size_t const nr_values_per_item = rank;

    std::string const& property_name = "space_discretization";
    hdf5::Shape const shape{nr_values_per_item};
    hdf5::Shape const chunk{nr_values_per_item};
    auto const file_datatype_id =
        hdf5::StandardDatatypeTraits<uint32_t>::type_id();
    auto const memory_datatype_id =
        hdf5::NativeDatatypeTraits<uint32_t>::type_id();

    auto property = omnipresent::same_shape::create_property(
        property_set, property_name,
        file_datatype_id, memory_datatype_id,
        shape, chunk);

    auto& nr_cells = property.reserve(nr_items);

    hdf5::Offset const offset(2, 0);
    hdf5::Stride const stride(2, 1);
    hdf5::Count const count{nr_items, nr_values_per_item};

    // TODO nr_cols is not written...
    nr_cells.write(offset, stride, count, discretization.shape());

    return property;
}

}  // Anonymous namespace


Raster::Discretization::Discretization(
    size_t const nr_rows,
    size_t const nr_cols)

    : _shape{nr_rows, nr_cols}

{
}


size_t const* Raster::Discretization::shape() const
{
    return _shape;
}


size_t Raster::Discretization::nr_rows() const
{
    return _shape[0];
}


size_t Raster::Discretization::nr_cols() const
{
    return _shape[1];
}


Raster::Domain::Domain(
    std::string const& crs,
    double const west,
    double const north,
    double const east,
    double const south)

    : _crs{crs},
      _coordinates{west, north, east, south}

{
}


double const* Raster::Domain::coordinates() const
{
    return _coordinates;
}


Raster::Raster(
    Dataset&& dataset,
    omnipresent::PropertySet&& property_set,
    omnipresent::same_shape::Property&& discretization)

    : _dataset{std::forward<Dataset>(dataset)},
      _property_set{std::forward<omnipresent::PropertySet>(property_set)},
      _discretization{std::forward<omnipresent::same_shape::Property>(
          discretization)}

{
}


// TODO blocked I/O, using a band type

// /*!
//     @brief      Write a band to the raster
//     @param      name Name of property
//     @param      datatype Datatype of the raster cells
//     @param      buffer Buffer with cell values
//     @exception  .
// 
//     This method creates a new property in the property set representing the
//     raster. The property is linked to the space discretization property which
//     is already in the dataset.
// */
// void Raster::write_band(
//     std::string const& name,
//     hdf5::Datatype const& datatype,
//     void const* buffer)
// {
//     size_t const rank = 2;
//     auto property = omnipresent::different_shape::create_property(
//         _property_set, name, datatype, rank);
// 
//     property.discretize_space(_discretization);
// 
//     // Reserve space for one item with the shape of the raster
//     // Write values
// 
// }


/*!
    @brief      Create a raster in a new LUE dataset
    @exception  std::runtime_error In case the dataset cannot be created

    If a dataset named @a dataset_name already exists, it is truncated.

    This function does the folowing:
    - Create a new empty [dataset](@ref Dataset), or truncate the existing one
    - Add a [phenomenon](@ref Phenomenon)
    - Add a [property set](@ref PropertySet) with a [domain](Domain)
      representing the time and space extent of the raster
*/
Raster create_raster(
    std::string const& dataset_name,
    std::string const& phenomenon_name,
    std::string const& property_set_name,
    Raster::Domain const& domain,
    Raster::Discretization const& discretization)
{
    // namespace omnipresent = constant_size::time::omnipresent;

    auto dataset = create_dataset(dataset_name);
    auto& phenomenon = dataset.add_phenomenon(phenomenon_name);
    auto property_set = omnipresent::create_property_set(phenomenon,
        property_set_name);
    auto const file_datatype_id =
        hdf5::StandardDatatypeTraits<double>::type_id();
    auto const memory_datatype_id =
        hdf5::NativeDatatypeTraits<double>::type_id();
    size_t const rank = 2;
    auto space_domain = omnipresent::configure_space_box_domain(property_set,
        file_datatype_id, memory_datatype_id, rank);

    hsize_t const nr_items = 1;

    auto& space_box = space_domain.reserve(nr_items);
    assert(space_box.shape().size() == rank);
    assert(space_box.shape()[0] == 1);
    assert(space_box.shape()[1] == 4);
    auto const nr_coordinates_per_box = space_box.shape()[1];

    hdf5::Offset const offset(rank, 0);
    hdf5::Stride const stride(rank, 1);
    hdf5::Count const count{nr_items, nr_coordinates_per_box};

    space_box.write(offset, stride, count, domain.coordinates());

    auto property = discretization_property(property_set, discretization);

    return Raster(std::move(dataset), std::move(property_set),
       std::move(property));
}

}  // namespace hl
}  // namespace lue

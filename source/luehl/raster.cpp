#include "luehl/raster.h"
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
        hdf5::StandardDatatypeTraits<hsize_t>::type_id();
    auto const memory_datatype_id =
        hdf5::NativeDatatypeTraits<hsize_t>::type_id();

    auto property = omnipresent::same_shape::create_property(
        property_set, property_name,
        file_datatype_id, memory_datatype_id,
        shape, chunk);

    auto& nr_cells = property.reserve(nr_items);

    nr_cells.write(discretization.shape());

    return property;
}

}  // Anonymous namespace


Raster::Discretization::Discretization()

    : _shape{0, 0}

{
}


Raster::Discretization::Discretization(
    hsize_t const nr_rows,
    hsize_t const nr_cols)

    : _shape{nr_rows, nr_cols}

{
}


hsize_t const* Raster::Discretization::shape() const
{
    return _shape;
}


hsize_t* Raster::Discretization::shape()
{
    return _shape;
}


hsize_t Raster::Discretization::nr_rows() const
{
    return _shape[0];
}


hsize_t Raster::Discretization::nr_cols() const
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


Raster::Band::Band(
    omnipresent::different_shape::Property&& property)

    : _property{std::forward<omnipresent::different_shape::Property>(property)}

{
}


void Raster::Band::write(
    hdf5::Dataspace const& memory_dataspace,
    hdf5::Offset const& start,
    hdf5::Stride const& stride,
    hdf5::Count const& count,
    void const* buffer)
{
    _property.values()[0].write(memory_dataspace, start, stride, count,
        buffer);
}


Raster::Raster(
    hdf5::Identifier const& id)

    : _property_set(id),
      _discretization_property(
          _property_set.properties()["space_discretization"],
          H5T_NATIVE_HSIZE),
      _discretization()

{
    _discretization_property.values().read(_discretization.shape());
}


// Raster::Raster(
//     Dataset& dataset,
//     std::string const& phenomenon_name,
//     std::string const& property_set_name)
// 
//     : _property_set(dataset.
//         phenomena()[phenomenon_name].
//         property_sets()[property_set_name])
//       _ omnipresent::same_shape::Property _discretization_property;
// 
// {
// 
// }
// 
// 
// Raster::Raster(
//     // std::shared_ptr<Dataset> const& dataset,
//     omnipresent::PropertySet&& property_set,
//     omnipresent::same_shape::Property&& discretization_property,
//     Raster::Discretization const& discretization)
// 
//     : // _dataset(dataset),
//       _property_set{std::forward<omnipresent::PropertySet>(property_set)},
//       _discretization_property{std::forward<omnipresent::same_shape::Property>(
//           discretization_property)},
//       _discretization{discretization}
// 
// {
// }


Raster::Band Raster::add_band(
    std::string const& name,
    hdf5::Datatype const& datatype)
{
    int const rank = 2;
    auto property = omnipresent::different_shape::create_property(
        _property_set, name, datatype, rank);
    size_t const nr_items = 1;

    property.reserve(nr_items, _discretization.shape());
    property.discretize_space(_discretization_property);

    return Band(std::move(property));
}


namespace {

// Raster create_raster(
//     PropertySet& property_set,
//     Raster::Domain const& domain,
//     Raster::Discretization const& discretization)
// {
// 
// 
// }


hdf5::Identifier create_raster(
    Phenomenon& phenomenon,
    std::string const& property_set_name,
    Raster::Domain const& domain,
    Raster::Discretization const& discretization)
{
    if(!phenomenon.property_sets().contains(property_set_name)) {

        auto property_set = omnipresent::create_property_set(
            phenomenon, property_set_name);


        hsize_t const nr_items = 1;

        // Write item id to property set.
        {
            hsize_t const id = 0;
            auto& ids = property_set.reserve(nr_items);
            ids.write(&id);
        }

        // Write raster extent to space domain.
        {
            auto const file_datatype_id =
                hdf5::StandardDatatypeTraits<double>::type_id();
            auto const memory_datatype_id =
                hdf5::NativeDatatypeTraits<double>::type_id();
            size_t const rank = 2;
            auto space_domain = omnipresent::configure_space_box_domain(
                property_set, file_datatype_id, memory_datatype_id, rank);

            auto& space_box = space_domain.reserve(nr_items);

            space_box.write(domain.coordinates());
        }

        // Write property with discretization information to property set.
        /* auto property = */ discretization_property(property_set, discretization);

        // return Raster(std::move(property_set), std::move(property),
        //     discretization);

        // return create_raster(property_set, domain, discretization);

        return property_set.id();
    }
    else {
        return phenomenon.property_sets()[property_set_name].id();
    }
}

}  // Anonymous namespace


Raster create_raster(
    Dataset& dataset,
    std::string const& phenomenon_name,
    std::string const& property_set_name,
    Raster::Domain const& domain,
    Raster::Discretization const& discretization)
{
    if(!dataset.phenomena().contains(phenomenon_name)) {
        return Raster(create_raster(
            dataset.add_phenomenon(phenomenon_name),
            property_set_name, domain, discretization));
    }
    else {
        return Raster(create_raster(
            dataset.phenomena()[phenomenon_name],
            property_set_name, domain, discretization));
    }
}


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


// Raster create_raster(
//     std::string const& dataset_name,
//     std::string const& phenomenon_name,
//     std::string const& property_set_name,
//     Raster::Domain const& domain,
//     Raster::Discretization const& discretization)
// {
//     return create_raster(create_dataset(dataset_name), phenomenon_name,
//         property_set_name, domain, discretization);
// 
// 
//     // auto& phenomenon = dataset.add_phenomenon(phenomenon_name);
//     // auto property_set = omnipresent::create_property_set(phenomenon,
//     //     property_set_name);
//     // hsize_t const nr_items = 1;
// 
// 
//     // // Write item id to property set.
//     // {
//     //     hsize_t const id = 0;
//     //     auto& ids = property_set.reserve(nr_items);
// 
//     //     ids.write(&id);
//     // }
// 
// 
//     // // Write raster extent to space domain.
//     // {
//     //     auto const file_datatype_id =
//     //         hdf5::StandardDatatypeTraits<double>::type_id();
//     //     auto const memory_datatype_id =
//     //         hdf5::NativeDatatypeTraits<double>::type_id();
//     //     size_t const rank = 2;
//     //     auto space_domain = omnipresent::configure_space_box_domain(
//     //         property_set, file_datatype_id, memory_datatype_id, rank);
// 
//     //     auto& space_box = space_domain.reserve(nr_items);
// 
//     //     space_box.write(domain.coordinates());
//     // }
// 
// 
//     // // Write property with discretization information to property set.
//     // auto property = discretization_property(property_set, discretization);
// 
// 
//     // return Raster(std::move(dataset), std::move(property_set),
//     //    std::move(property), discretization);
// }

}  // namespace hl
}  // namespace lue

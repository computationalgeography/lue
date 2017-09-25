#include "lue/hl/raster.hpp"
#include "lue/constant_size/time/omnipresent/space_box_domain.hpp"
#include "lue/hdf5/datatype_traits.hpp"


namespace lue {
namespace hl {
namespace {

std::string const discretization_property_name = "_space_discretization";

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


Raster::Domain::Domain()

    : _crs{},
      _coordinates{{0.0, 0.0, 0.0, 0.0}}

{
}


Raster::Domain::Domain(
    std::string const& crs,
    double const west,
    double const south,
    double const east,
    double const north)

    : _crs{crs},
      _coordinates{{west, south, east, north}}

{
}


std::string const& Raster::Domain::crs() const
{
    return _crs;
}


double Raster::Domain::west() const
{
    return _coordinates[0];
}


double Raster::Domain::south() const
{
    return _coordinates[1];
}


double Raster::Domain::east() const
{
    return _coordinates[2];
}


double Raster::Domain::north() const
{
    return _coordinates[3];
}


std::array<double, 4>& Raster::Domain::coordinates()
{
    return _coordinates;
}


std::array<double, 4> const& Raster::Domain::coordinates() const
{
    return _coordinates;
}


Raster::Band::Band(
    omnipresent::different_shape::Property&& property)

    : _property{std::forward<omnipresent::different_shape::Property>(property)}

{
}


/*!
    @brief      Return in-file datatype of the cell values
*/
hdf5::Datatype Raster::Band::file_datatype() const
{
    return _property.values().file_datatype();
}


/*!
    @brief      Return in-memory datatype of the cell values
*/
hdf5::Datatype Raster::Band::memory_datatype() const
{
    return _property.values().memory_datatype();
}


void Raster::Band::read(
    void* buffer) const
{
    _property.values()[0].read(buffer);
}


void Raster::Band::read(
    hdf5::Hyperslab const& hyperslab,
    void* buffer) const
{
    _property.values()[0].read(hyperslab, buffer);
}


/*!
    @brief      Write values in @a buffer to raster band
    @warning    The values pointed to by @a buffer must have the same type
                as the memory_datatype()
    @warning    The number of values pointed to by @a buffer must be at least
                as large as the number of cells in the raster
*/
void Raster::Band::write(
    void const* buffer)
{
    _property.values()[0].write(buffer);
}


/*!
    @brief      Write values to hyperslab
    @warning    The values pointed to by @a buffer must have the same type
                as the memory_datatype()
    @warning    The number of values pointed to by @a buffer must be at least
                as large as the size of the hyperslab
*/
void Raster::Band::write(
    hdf5::Dataspace const& memory_dataspace,
    hdf5::Hyperslab const& hyperslab,
    void const* buffer)
{
    _property.values()[0].write(memory_dataspace, hyperslab, buffer);
}


/*!
    @brief      Construct an instance
    @param      id Identifier of property set representing the raster

    It is assumed that the property set contains a property named
    'space_discretization' representing the raster's discretization.
*/
Raster::Raster(
    hdf5::Identifier const& id)

    : _property_set(id),
      _discretization_property(
          _property_set.properties()[discretization_property_name],
          H5T_NATIVE_HSIZE),
      _discretization(),
      _domain()

{
    _discretization_property.values().read(_discretization.shape());

    auto const& space_domain = _property_set.domain().space();
    auto const space_box_domain = omnipresent::SpaceBoxDomain(space_domain,
        hdf5::NativeDatatypeTraits<double>::type_id());
    auto const& space_boxes = space_box_domain.boxes();
    assert(space_boxes.nr_items() == 1);
    space_boxes.read(_domain.coordinates().data());
}


Raster::Raster(
    Dataset& dataset,
    std::string const& phenomenon_name,
    std::string const& property_set_name)

    : _property_set(
            dataset
                .phenomena()[phenomenon_name]
                    .property_sets()[property_set_name].id()),
      _discretization_property(
          _property_set.properties()[discretization_property_name],
          H5T_NATIVE_HSIZE),
      _discretization(),
      _domain()

{
    _discretization_property.values().read(_discretization.shape());

    auto const& space_domain = _property_set.domain().space();
    auto const space_box_domain = omnipresent::SpaceBoxDomain(space_domain,
        hdf5::NativeDatatypeTraits<double>::type_id());
    auto const& space_boxes = space_box_domain.boxes();
    assert(space_boxes.nr_items() == 1);
    space_boxes.read(_domain.coordinates().data());
}


Raster::Domain const& Raster::domain() const
{
    return _domain;
}


Raster::Discretization const& Raster::discretization() const
{
    return _discretization;
}


/*!
    @brief      Add a band to the raster
    @param      name Name of band to add
    @param      datatype Memory datatype of cell values
*/
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


Raster::Band Raster::band(
    std::string const& name) const
{
    return Band(
        omnipresent::different_shape::Property(
            _property_set.properties()[name]));
}


namespace {

/*!
    @brief      Add property representing a raster's discretization to
                @a property_set
    @param      .
    @return     .
    @exception  .
*/
omnipresent::same_shape::Property discretization_property(
    omnipresent::PropertySet& property_set,
    Raster::Discretization const& discretization)
{
    size_t const nr_items = 1;

    // nr_items x 1D value.
    size_t const rank = 2;

    // Two values per item: nr_rows, nr_cols.
    size_t const nr_values_per_item = rank;

    std::string const& property_name = discretization_property_name;
    hdf5::Shape const shape{nr_values_per_item};
    auto const file_datatype_id =
        hdf5::StandardDatatypeTraits<hsize_t>::type_id();
    auto const memory_datatype_id =
        hdf5::NativeDatatypeTraits<hsize_t>::type_id();

    auto property = omnipresent::same_shape::create_property(
        property_set, property_name,
        file_datatype_id, memory_datatype_id, shape);

    auto& nr_cells = property.reserve(nr_items);

    nr_cells.write(discretization.shape());

    return property;
}


/*!
    @brief      Add representation of a raster to @a phenomenon
    @param      phenomenon Phenomenon to add raster to
    @param      property_set_name Name of raster to add
    @param      domain Domain of raster
    @param      discretization Discretization of raster
    @return     Identifier of property_set representing the raster

    If no property-set with the name passed in exists in @a phenomenon,
    it will be added to the phenomenon.
*/
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

            space_box.write(domain.coordinates().data());
        }

        // Write property with discretization information to property set.
        /* auto property = */ discretization_property(
            property_set, discretization);

        return property_set.id();
    }
    else {
        return phenomenon.property_sets()[property_set_name].id();
    }
}

}  // Anonymous namespace


// bool raster_exists(
//     Dataset const& dataset,
//     std::string const& phenomenon_name,
//     std::string const& property_set_name)
// {
//     bool result = false;
// 
//     if(dataset.phenomena().contains(phenomenon_name)) {
//         auto const& phenomenon = dataset.phenomena()[phenomenon_name];
// 
//         if(phenomenon.property_sets().contains(property_set_name)) {
//             auto const& property_set =
//                 phenomenon.property_sets()[property_set_name];
// 
//             if(omnipresent::space_box_domain_exists(property_set) /* &&
//                     discretization_property_exists(property_set) */) {
//                 result = true;
//             }
//         }
//     }
// 
//     return result;
// }


/*!
    @brief      Add representation of a raster to @a dataset
    @param      dataset Dataset to add raster to
    @param      phenomenon_name Name of phenomenon to add raster to
    @param      property_set_name Name of raster to add
    @param      domain Domain of raster
    @param      discretization Discretization of raster

    If no phenomenon with the name passed in exists in @a dataset,
    it will be added to the dataset.
*/
Raster create_raster(
    Dataset& dataset,
    std::string const& phenomenon_name,
    std::string const& property_set_name,
    Raster::Domain const& domain,
    Raster::Discretization const& discretization)
{
    assert(property_set_name != discretization_property_name);

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

}  // namespace hl
}  // namespace lue

#include "lue/hl/raster_stack.hpp"
#include "lue/constant_size/time/located/shared/time_box_domain.hpp"
#include "lue/constant_size/time/located/shared/constant_shape/same_shape/property.hpp"
#include "lue/constant_size/time/omnipresent/property_set.hpp"
#include "lue/constant_size/time/omnipresent/space_box_domain.hpp"
// #include "lue/hdf5/datatype_traits.hpp"
#include <numeric>


namespace lue {
namespace hl {
namespace {

std::string const space_box_property_set_name = "space_boxes";

std::string const time_discretization_phenomenon_name = "time_boxes";
std::string const time_discretization_property_set_name = "discretization";
std::string const time_discretization_property_name = "nr_steps";

std::string const space_discretization_phenomenon_name = "space_boxes";
std::string const space_discretization_property_set_name = "discretization";
std::string const space_discretization_property_name = "nr_cells";

std::string const time_discretization_link_name = "time_discretization";
std::string const space_discretization_link_name = "space_discretization";

}  // Anonymous namespace


RasterStack::Band::Band(
    shared::constant_shape::different_shape::Property&& property)

    : _property{std::forward<shared::constant_shape::different_shape::Property>(
          property)}

{
}


/*!
    @brief      Return in-file datatype of the cell values
*/
hdf5::Datatype RasterStack::Band::file_datatype() const
{
    return _property.values().file_datatype();
}


/*!
    @brief      Return in-memory datatype of the cell values
*/
hdf5::Datatype RasterStack::Band::memory_datatype() const
{
    return _property.values().memory_datatype();
}


void RasterStack::Band::read(
    void* buffer) const
{
    _property.values()[0].read(buffer);
}


// void RasterStack::Band::read(
//     hdf5::Hyperslab const& hyperslab,
//     void* buffer) const
// {
//     _property.values()[0].read(hyperslab, buffer);
// }


void RasterStack::Band::write(
    void const* buffer)
{
    _property.values()[0].write(buffer);
}


void RasterStack::Band::write(
    hdf5::Dataspace const& memory_dataspace,
    hdf5::Hyperslab const& hyperslab,
    void const* buffer)
{
    _property.values()[0].write(memory_dataspace, hyperslab, buffer);
}


// /*!
//     @brief      Write values in @a buffer to raster band
//     @warning    The values pointed to by @a buffer must have the same type
//                 as the memory_datatype()
//     @warning    The number of values pointed to by @a buffer must be at least
//                 as large as the number of cells in the raster
// */
// void RasterStack::Band::write(
//     void const* buffer)
// {
//     _property.values()[0].write(buffer);
// }
// 
// 
// /*!
//     @brief      Write values to hyperslab
//     @warning    The values pointed to by @a buffer must have the same type
//                 as the memory_datatype()
//     @warning    The number of values pointed to by @a buffer must be at least
//                 as large as the size of the hyperslab
// */
// void RasterStack::Band::write(
//     hdf5::Dataspace const& memory_dataspace,
//     hdf5::Hyperslab const& hyperslab,
//     void const* buffer)
// {
//     _property.values()[0].write(memory_dataspace, hyperslab, buffer);
// }


RasterStack::RasterStack(
    hdf5::Identifier const& phenomenon_id,
    std::string const& property_set_name)
    // hdf5::Identifier const& time_discretization_property_id,
    // hdf5::Identifier const& space_discretization_property_id)

    : _phenomenon{hdf5::Group{phenomenon_id}},
      _property_set{
          hdf5::Group{_phenomenon.property_sets()[property_set_name].id()}},
      _time_discretization_property{
          hdf5::Group{_property_set, time_discretization_link_name},
          hdf5::Datatype{hdf5::NativeDatatypeTraits<hsize_t>::type_id()}},
      _space_discretization_property{
          hdf5::Group{_property_set, space_discretization_link_name},
          hdf5::Datatype{hdf5::NativeDatatypeTraits<hsize_t>::type_id()}},
      _domain{},
      _discretization{}

{
    read();
}


// RasterStack::RasterStack(
//     hdf5::Identifier const& phenomenon_id,
//     std::string const& property_set_name)
// 
//     : _phenomenon{phenomenon_id},
//       _property_set{_phenomenon.property_sets()[property_set_name].id()},
//       _time_discretization_property{},
//       _space_discretization_property{
//           space_discretization_property_id,
//           hdf5::NativeDatatypeTraits<hsize_t>::type_id()},
//       // _time_domain{},
//       // _space_domain{},
//       _discretization{}
// 
// {
// 
// 
// 
// 
// }


void RasterStack::read()
{
    // Domain
    {
        // Time box domain
        shared::TimeBoxDomain time_box_domain{_property_set.domain()};
        TimeSeriesDomain::Coordinates time_coordinates;
        time_box_domain.items().read(time_coordinates.data());
        TimeSeriesDomain time_domain{
            time_box_domain.configuration().clock(),
            std::move(time_coordinates)};

        // Space point domain
        auto const& property_set = 
            _phenomenon.property_sets()[space_box_property_set_name];
        omnipresent::SpaceBoxDomain space_box_domain{
            property_set.domain(),
            hdf5::Datatype{
                hdf5::NativeDatatypeTraits<RasterDomain::Coordinate>
                    ::type_id()}};
        RasterDomain::Coordinates space_coordinates;
        space_box_domain.items().read(space_coordinates.data());
        RasterDomain space_domain{"blah", std::move(space_coordinates)};

        _domain = RasterStackDomain{time_domain, space_domain};
    }


    // Discretization
    {
        // Time discretization
        assert(_time_discretization_property.values().nr_items() == 1);
        TimeSeriesDiscretization::Shape time_box_shape;
        _time_discretization_property.values().read(time_box_shape.data());
        TimeSeriesDiscretization time_discretization{std::move(time_box_shape)};

        // Space discretization
        assert(_space_discretization_property.values().nr_items() == 1);
        RasterDiscretization::Shape space_box_shape;
        _space_discretization_property.values().read(space_box_shape.data());
        RasterDiscretization space_discretization{std::move(space_box_shape)};

        _discretization = RasterStackDiscretization{
            time_discretization, space_discretization};
    }
}


RasterStackDomain const& RasterStack::domain() const
{
    return _domain;
}


RasterStackDiscretization const& RasterStack::discretization() const
{
    return _discretization;
}


/*!
    @brief      Add a band to the raster
    @param      name Name of band to add
    @param      datatype Memory datatype of cell values
*/
RasterStack::Band RasterStack::add_band(
    std::string const& name,
    hdf5::Datatype const& datatype)
{
    int const rank = 3;  // [nr_steps, nr_rows, nr_cols]
    auto property = shared::constant_shape::different_shape::create_property(
        _property_set, name, datatype, rank);

    hsize_t const nr_time_domain_items = 1;
    size_t const nr_items = 1;

    property.values().reserve(
        nr_time_domain_items, nr_items, _discretization.shape().data());
    property.discretize_time(_time_discretization_property);
    property.discretize_space(_space_discretization_property);

    return Band(std::move(property));
}


RasterStack::Band RasterStack::band(
    std::string const& name) const
{
    return Band{shared::constant_shape::different_shape::Property{
        hdf5::Group{_property_set.properties()[name].id()}}};
}


namespace {

RasterStack create_raster_stack(
    Phenomenon& phenomenon,
    Phenomenon& time_discretization_phenomenon,
    Phenomenon& space_discretization_phenomenon,
    std::string const& property_set_name,
    RasterStackDomain const& domain,
    RasterStackDiscretization const& discretization)
{
    // hdf5::Identifier time_discretization_property_id;
    // hdf5::Identifier space_discretization_property_id;

    if(!phenomenon.property_sets().contains(property_set_name)) {

        hdf5::Datatype file_datatype_id;
        hdf5::Datatype memory_datatype_id;

        // One time box
        std::size_t const nr_time_boxes = 1;

        // One space box
        hsize_t const nr_items = 1;

        auto property_set = shared::create_property_set(
            phenomenon.property_sets(), property_set_name);


        // Write item ids to property set
        auto ids_ = std::make_unique<hsize_t[]>(nr_items);
        std::iota(ids_.get(), ids_.get() + nr_items, 0);
        auto& ids = property_set.ids();
        ids.reserve(nr_items);
        ids.write(ids_.get());


        // Create time-box domain
        auto time_box_domain = shared::create_time_box_domain(
            property_set, domain.time_series_domain().clock());
        auto& time_boxes = time_box_domain.reserve(nr_time_boxes);
        time_boxes.write(domain.time_series_domain().coordinates().data());


        // Create property set for storing space boxes. These don't change over
        // time, so they need to be stored in a different property set
        // (omnipresent) than the one with the time series values (located).
        {
            // auto const& ids = property_set.ids();
            auto property_set = omnipresent::create_property_set(
                phenomenon, space_box_property_set_name, ids);
            file_datatype_id = hdf5::Datatype{
                hdf5::StandardDatatypeTraits<RasterDomain::Coordinate>::
                    type_id()};
            memory_datatype_id = hdf5::Datatype{
                hdf5::NativeDatatypeTraits<RasterDomain::Coordinate>
                    ::type_id()};
            auto space_domain = omnipresent::create_space_box_domain(
                property_set, file_datatype_id, memory_datatype_id, 2);
            space_domain.reserve(nr_items).write(
                domain.raster_domain().coordinates().data());
        }




        // Create property for storing time discretization information. This
        // information is shared by all items, and doesn't change over time.

        /// file_datatype_id = hdf5::StandardDatatypeTraits<hsize_t>::type_id();
        /// memory_datatype_id = hdf5::NativeDatatypeTraits<hsize_t>::type_id();
        /// auto time_discretization_property =
        ///     shared::constant_shape::same_shape::create_property(
        ///         property_set, time_discretization_property_name,
        ///         file_datatype_id, memory_datatype_id);
        /// time_discretization_property.reserve(nr_time_boxes, nr_items).write(
        ///     discretization.time_series_discretization().shape().data());

        auto time_discretization_property_set = omnipresent::create_property_set(
            time_discretization_phenomenon,
            time_discretization_property_set_name);

        // Write item ids to property set
        {
            hsize_t const id = 0;
            auto& ids = time_discretization_property_set.ids();
            ids.reserve(nr_items);
            ids.write(&id);
        }

        // Add property
        file_datatype_id = hdf5::Datatype{
            hdf5::StandardDatatypeTraits<hsize_t>::type_id()};
        memory_datatype_id = hdf5::Datatype{
            hdf5::NativeDatatypeTraits<hsize_t>::type_id()};
        auto time_discretization_property =
            omnipresent::same_shape::create_property(
                time_discretization_property_set,
                time_discretization_property_name,
                file_datatype_id,
                memory_datatype_id,
                hdf5::Shape{1});
        time_discretization_property.values().reserve(1);
        time_discretization_property.values().write(
            discretization.time_series_discretization().shape().data());
        // time_discretization_property_id = time_discretization_property.id();


        // Create property set and property for storing space
        // discretization information. This information does not change
        // through time.
        auto space_discretization_property_set =
            omnipresent::create_property_set(
                space_discretization_phenomenon,
                space_discretization_property_set_name);

        // Write item ids to property set
        {
            hsize_t const id = 0;
            space_discretization_property_set.ids().reserve(nr_items);
            space_discretization_property_set.ids().write(&id);
        }

        // Two values per item: nr_rows, nr_cols.
        file_datatype_id = hdf5::Datatype{
            hdf5::StandardDatatypeTraits<hsize_t>::type_id()};
        memory_datatype_id = hdf5::Datatype{
            hdf5::NativeDatatypeTraits<hsize_t>::type_id()};
        auto space_discretization_property =
            omnipresent::same_shape::create_property(
                space_discretization_property_set,
                space_discretization_property_name,
                file_datatype_id, memory_datatype_id, hdf5::Shape{2});
        space_discretization_property.values().reserve(nr_items);
        space_discretization_property.values().write(
            discretization.raster_discretization().shape().data());
        // space_discretization_property_id = space_discretization_property.id();

        // // Create property for storing raster stack values
        // // TODO Switch
        // file_datatype_id = hdf5::StandardDatatypeTraits<double>::type_id();
        // memory_datatype_id = hdf5::NativeDatatypeTraits<double>::type_id();
        // auto value_property =
        //     shared::constant_shape::same_shape::create_property(
        //         property_set, property_name,
        //         file_datatype_id, memory_datatype_id);


        // value_property.discretize_time(time_discretization_property);
        // value_property.discretize_space(space_discretization_property);

        // We are not creating properties here yet. To be able to do so later
        // on (add_band), we need to create links to the time and space
        // discretizations.

        property_set.create_soft_link(
            time_discretization_property.id(), time_discretization_link_name);
        property_set.create_soft_link(
            space_discretization_property.id(), space_discretization_link_name);
    }

    return RasterStack{phenomenon.id(), property_set_name};
        // time_discretization_property_id, space_discretization_property_id};
}

}  // Anonymous namespace


bool raster_stack_exists(
    Dataset& dataset,
    std::string const& phenomenon_name,
    std::string const& property_set_name)
{
    bool result = false;

    if(dataset.phenomena().contains(phenomenon_name)) {
        auto const& phenomenon = dataset.phenomena()[phenomenon_name];

        if(phenomenon.property_sets().contains(property_set_name)) {
            auto const& property_set =
                phenomenon.property_sets()[property_set_name];

            if(property_set.contains_soft_link(
                        time_discretization_link_name) &&
                    property_set.contains_soft_link(
                        space_discretization_link_name)) {

                // TODO More tests?
                result = true;

            }
        }
    }

    return result;
}


RasterStack create_raster_stack(
    Dataset& dataset,
    std::string const& phenomenon_name,
    std::string const& property_set_name,
    RasterStackDomain const& domain,
    RasterStackDiscretization const& discretization)
{
    assert(property_set_name != time_discretization_property_name);

    if(dataset.phenomena().contains(phenomenon_name)) {
        throw std::runtime_error(
            "Importing raster stack in existing property-set is not supported "
            "yet");
    }

    Phenomenon& phenomenon = !dataset.phenomena().contains(phenomenon_name)
        ? dataset.add_phenomenon(phenomenon_name)
        : dataset.phenomena()[phenomenon_name]
        ;

    auto result = create_raster_stack(
        phenomenon,

        // This can also be a phenomenon with a temp name. As long as one
        // with that name doesn't already exist.
        dataset.add_phenomenon(time_discretization_phenomenon_name),
        dataset.add_phenomenon(space_discretization_phenomenon_name),

        property_set_name, domain, discretization);

    return result;
}

}  // namespace hl
}  // namespace lue

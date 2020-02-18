#include "lue/data_model/hl/raster_view.hpp"
#include <fmt/format.h>


namespace lue {
namespace {

static std::string const time_discretization_property_name{"time_grid_shape"};

static std::string const space_discretization_property_name{"space_grid_shape"};

}  // Anonymous namespace

// Modelling use-case
// - Initialize state using existing data
// - Simulate state changes through time
//
// IO requirements, simplest case
// - Create new dataset
// - Initialize dataset for modelled state
// - For each location in time, write data to dataset
// - Also support writing static rasters
//
// High(er) level I/O library, that assumes certain use-cases
// - PCRaster style data
//     - Single phenomenon
//     - A property-set for storing raster slices
//         - Single object
//         - One or more temporal spatial properties
//         - One or more constant spatial properties
//         - Single discretized time box + property
//         - Stationary discretized space boxes + property
//
// Pseudo code
// - property_a = dataset.add_property(property_a_name, datatype);
// - property_b = dataset.add_property(property_b_name, datatype);


/*!
    @brief      Construct an instance
    @param      dataset Dataset to manage

    It is assumed that @a dataset is already layed out per this class'
    conventions. Typically, it is the result of calling
    create_raster_view(), or similar.
*/
RasterView::RasterView(
    DatasetPtr dataset_ptr,
    std::string const& phenomenon_name,
    std::string const& property_set_name):

    DatasetView{std::move(dataset_ptr)},
    _phenomenon_name{phenomenon_name},
    _property_set_name{property_set_name},
    _id{999},
    _time_box(),
    _time_grid(1),
    _space_box(),
    _space_grid(2),
    _layer_names{}

{
    Dataset& dataset{**this};
    auto& phenomenon{dataset.phenomena()[_phenomenon_name]};
    auto& property_set{phenomenon.property_sets()[_property_set_name]};

    assert(property_set.object_tracker().active_object_id().nr_ids() == 1);
    property_set.object_tracker().active_object_id().read(0, &_id);

    // Read time and space boxes from domains
    {
        auto& time_domain{property_set.time_domain()};
        auto value{time_domain.value<lue::TimeBox>()};
        value.read(0, _time_box.data());
    }

    {
        auto& space_domain{property_set.space_domain()};
        auto value{space_domain.value<StationarySpaceBox>()};
        value.read(0, _space_box.data());
    }

    // Open discretization properties and read information from them
    auto& properties{property_set.properties()};
    auto& time_discretization_property =
        properties.collection<same_shape::Properties>()[
            time_discretization_property_name];
    time_discretization_property.value().read(0, _time_grid.data());

    auto& space_discretization_property =
        properties.collection<same_shape::Properties>()[
            space_discretization_property_name];
    space_discretization_property.value().read(0, _space_grid.data());

    // Scan for properties that represent the rasters
    {
        using RasterProperties = different_shape::constant_shape::Properties;
        using RasterProperty = different_shape::constant_shape::Property;

        RasterProperties& raster_properties{
            properties.collection<RasterProperties>()};

        for(auto const& name: raster_properties.names()) {

            RasterProperty& property{raster_properties[name]};

            // Perform some sanity checks on the properties to skip the
            // ones that actually aren't raster properties
            bool const property_is_discretized = 
                property.time_is_discretized() &&
                property.time_discretization_type() ==
                    TimeDiscretization::regular_grid &&
                property.space_is_discretized() &&
                property.space_discretization_type() ==
                    SpaceDiscretization::regular_grid;

            if(property_is_discretized) {

                bool const time_discretization_properties_match = 
                    property.time_discretization_property().id() == 
                        time_discretization_property.id();
                bool const space_discretization_properties_match = 
                    property.space_discretization_property().id() == 
                        space_discretization_property.id();

                if(time_discretization_properties_match &&
                        space_discretization_properties_match) {

                    _layer_names.push_back(name);
                }
            }
        }
    }
}


std::string const& RasterView::phenomenon_name() const
{
    return _phenomenon_name;
}


std::string const& RasterView::property_set_name() const
{
    return _property_set_name;
}


RasterView::TimeBox const& RasterView::time_box() const
{
    return _time_box;
}


RasterView::SpaceBox const& RasterView::space_box() const
{
    return _space_box;
}

 
hdf5::Shape::value_type RasterView::nr_time_steps() const
{
    return _time_grid[0];
}


hdf5::Shape const& RasterView::raster_shape() const
{
    return _space_grid;
}


Count RasterView::nr_layers() const
{
    return _layer_names.size();
}


std::vector<std::string> RasterView::layer_names() const
{
    return _layer_names;
}


bool RasterView::contains(
    std::string const& name)
{
    return std::find(_layer_names.begin(), _layer_names.end(), name) !=
        _layer_names.end();
}


RasterView::Layer RasterView::add_layer(
    std::string const& name,
    hdf5::Datatype const& datatype)
{
    if(contains(name)) {
        throw std::runtime_error(
            fmt::format("Raster layer {} already exists", name));
    }

    Dataset& dataset{**this};
    auto& phenomenon{dataset.phenomena()[_phenomenon_name]};
    auto& property_set{phenomenon.property_sets()[_property_set_name]};
    auto& properties{property_set.properties()};

    auto& time_discretization_property =
        properties.collection<same_shape::Properties>()[
            time_discretization_property_name];
    auto& space_discretization_property =
        properties.collection<same_shape::Properties>()[
            space_discretization_property_name];

    using RasterProperties = different_shape::constant_shape::Properties;
    using RasterProperty = different_shape::constant_shape::Property;

    RasterProperties& raster_properties{
        properties.collection<RasterProperties>()};

    assert(!raster_properties.contains(name));

    RasterProperty& raster_property{raster_properties.add(name, datatype, 2)};

    Layer layer{raster_property.value().expand(_id, _space_grid, _time_grid[0])};

    raster_property.set_time_discretization(
        TimeDiscretization::regular_grid, time_discretization_property);
    raster_property.set_space_discretization(
        SpaceDiscretization::regular_grid, space_discretization_property);

    _layer_names.push_back(name);

    assert(contains(name));

    return layer;
}


/*!
    @brief      Create a RasterView instance
    @param      .
    @return     .
    @exception  .
*/
RasterView create_raster_view(
    DatasetPtr dataset_ptr,
    std::string const& phenomenon_name,
    std::string const& property_set_name,
    Clock const& clock,
    Count nr_time_steps,
    RasterView::TimeBox const& time_box,
    hdf5::Shape const& raster_shape,
    RasterView::SpaceBox const& space_box)
{
    // → High-level data model code: lue::data_model_hl
    // - RasterView dataset = create_raster_dataset(
    //         dataset_pathname, clock, time_box, space_box);

    // Configure dataset to simplify adding rasters to it
    // Afterwards it must be easy to add
    // - constant rasters
    // - temporal rasters

    // Assumptions:
    // - single object
    //     - single stationary space extent, discretized
    //     - single time extent, discretized


    Dataset& dataset{*dataset_ptr};

    // Add one phenomenon
    auto& phenomenon = dataset.add_phenomenon(phenomenon_name);

    // Add the ID of the only object
    ID object_id{5};
    phenomenon.object_id().expand(1);
    phenomenon.object_id().write(0, &object_id);

    TimeConfiguration time_configuration{
            TimeDomainItemType::box
        };
    SpaceConfiguration space_configuration{
            Mobility::stationary,
            SpaceDomainItemType::box
        };

    static_assert(std::is_same_v<RasterView::SpaceBox::value_type, double>);
    auto& property_set = phenomenon.property_sets().add(
        property_set_name, time_configuration, clock,
        space_configuration, hdf5::native_float64, 2);

    // Object tracker
    auto& object_tracker{property_set.object_tracker()};

    Index active_set_idx{0};
    object_tracker.active_set_index().expand(1);
    object_tracker.active_set_index().write(0, &active_set_idx);

    object_tracker.active_object_id().expand(1);
    object_tracker.active_object_id().write(0, &object_id);

    // Time domain
    auto time_domain_value{property_set.time_domain().value<lue::TimeBox>()};
    time_domain_value.expand(1);
    time_domain_value.write(0, time_box.data());

    // Space domain
    auto space_domain_value{
        property_set.space_domain().value<StationarySpaceBox>()};
    space_domain_value.expand(1);
    space_domain_value.write(0, space_box.data());

    // Discretization properties
    Properties& properties{property_set.properties()};

    hdf5::Datatype const shape_datatype{
        hdf5::NativeDatatypeTraits<hdf5::Shape::value_type>::type_id()};

    {
        // Add property for storing time discretization
        // using Property = lue::same_shape::constant_shape::Property;
        auto& property = properties.add<same_shape::Property>(
            time_discretization_property_name, shape_datatype,
            lue::hdf5::Shape{1});  // nr_time_steps
        auto& value = property.value();
        value.expand(1);
        value.write(0, &nr_time_steps);
    }

    {
        // Add property for storing space discretization
        auto& property = properties.add<same_shape::Property>(
            space_discretization_property_name, shape_datatype,
            lue::hdf5::Shape{2});  // nr_rows, nr_cols
        auto& value = property.value();
        value.expand(1);
        value.write(0, raster_shape.data());
    }

    return RasterView{
        std::move(dataset_ptr), phenomenon_name, property_set_name};
}

}  // namespace lue

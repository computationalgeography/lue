#include "lue/data_model/hl/raster_view.hpp"
#include <fmt/format.h>


namespace lue {
namespace data_model {
namespace {

std::string const time_discretization_property_name{"time_grid_shape"};

std::string const space_discretization_property_name{"space_grid_shape"};

}  // Anonymous namespace


template<
    typename DatasetPtr>
RasterView<DatasetPtr>::RasterView(
    DatasetPtr dataset_ptr,
    std::string const& phenomenon_name,
    std::string const& property_set_name):

    DatasetView<DatasetPtr>{std::move(dataset_ptr)},
    _phenomenon_name{phenomenon_name},
    _property_set_name{property_set_name},
    _object_id{999},
    _space_box(),
    _space_grid(2),
    _layer_names{}

{
    Dataset& dataset{**this};
    auto& phenomenon{dataset.phenomena()[_phenomenon_name]};
    auto& property_set{phenomenon.property_sets()[_property_set_name]};

    phenomenon.object_id().read(0, &_object_id);

    // Read space box from domain
    {
        auto& space_domain{property_set.space_domain()};
        // cppcheck-suppress internalAstError
        auto value{space_domain.template value<StationarySpaceBox>()};
        value.read(0, _space_box.data());
    }

    // Open space discretization property and read information
    {
        auto& properties{property_set.properties()};
        auto& space_discretization_property =
            properties.template collection<same_shape::Properties>()[
                space_discretization_property_name];
        space_discretization_property.value().read(0, _space_grid.data());
    }
}


template<
    typename DatasetPtr>
std::string const& RasterView<DatasetPtr>::phenomenon_name() const
{
    return _phenomenon_name;
}


template<
    typename DatasetPtr>
std::string const& RasterView<DatasetPtr>::property_set_name() const
{
    return _property_set_name;
}


template<
    typename DatasetPtr>
ID RasterView<DatasetPtr>::object_id() const
{
    return _object_id;
}


template<
    typename DatasetPtr>
typename RasterView<DatasetPtr>::SpaceBox const&
    RasterView<DatasetPtr>::space_box() const
{
    return _space_box;
}


template<
    typename DatasetPtr>
hdf5::Shape const& RasterView<DatasetPtr>::grid_shape() const
{
    return _space_grid;
}


template<
    typename DatasetPtr>
Count RasterView<DatasetPtr>::nr_layers() const
{
    return _layer_names.size();
}


template<
    typename DatasetPtr>
std::vector<std::string> const& RasterView<DatasetPtr>::layer_names() const
{
    return _layer_names;
}


template<
    typename DatasetPtr>
bool RasterView<DatasetPtr>::contains(
    std::string const& name)
{
    return std::find(_layer_names.begin(), _layer_names.end(), name) !=
        _layer_names.end();
}


template<
    typename DatasetPtr>
void RasterView<DatasetPtr>::add_layer(
    std::string const& name)
{
    assert(!this->contains(name));

    _layer_names.push_back(name);

    assert(this->contains(name));
}


bool contains_raster(
    Dataset const& dataset,
    std::string const& phenomenon_name,
    std::string const& property_set_name)
{
    bool result{false};

    if(!dataset.phenomena().contains(phenomenon_name))
    {
        throw std::runtime_error(fmt::format(
            "Dataset {} does not contain phenomenon {}",
            dataset.pathname(), phenomenon_name));
    }

    auto const& phenomenon{dataset.phenomena()[phenomenon_name]};

    if(!phenomenon.property_sets().contains(property_set_name))
    {
        throw std::runtime_error(fmt::format(
            "Phenomenon {} does not contain property-set {}",
            phenomenon_name, property_set_name));
    }

    if(phenomenon.object_id().nr_objects() == 1)
    {
        auto const& property_set{
            phenomenon.property_sets()[property_set_name]};

        bool space_domain_ok{false};
        bool space_discretization_ok{false};

        // Space domain
        {
            if(property_set.has_space_domain())
            {
                auto const& space_domain{property_set.space_domain()};
                auto const& configuration{space_domain.configuration()};

                if(configuration.value<Mobility>() ==
                        Mobility::stationary &&
                    configuration.value<SpaceDomainItemType>() ==
                        SpaceDomainItemType::box &&
                    !space_domain.presence_is_discretized())
                {
                    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
                    auto space_box = const_cast<SpaceDomain&>(space_domain)
                        .value<StationarySpaceBox>();

                    if(space_box.nr_boxes() == 1)
                    {
                        space_domain_ok = true;
                    }
                }
            }
        }

        // Space discretization property
        if(space_domain_ok)
        {
            auto const& properties{property_set.properties()};

            if(properties.contains(space_discretization_property_name))
            {
                if( properties.shape_per_object(
                            space_discretization_property_name) ==
                        ShapePerObject::same &&
                    properties.value_variability(
                            space_discretization_property_name) ==
                        ValueVariability::constant)
                {
                    assert(
                        properties.collection<same_shape::Properties>().contains(
                            space_discretization_property_name));

                    auto const& discretization_property{
                        properties.collection<same_shape::Properties>()[
                            space_discretization_property_name]};
                    auto const& value{discretization_property.value()};

                    if(!discretization_property.time_is_discretized() &&
                        !discretization_property.space_is_discretized() &&
                        value.nr_arrays() == 1 &&
                        value.array_shape() == hdf5::Shape{2} &&
                        value.file_datatype() == hdf5::Datatype{
                            hdf5::StandardDatatypeTraits<
                                hdf5::Shape::value_type>::type_id()})
                    {
                        space_discretization_ok = true;
                    }
                }
            }
        }

        result = space_domain_ok && space_discretization_ok;
    }

    return result;
}


namespace constant {

/*!
    @brief      Construct an instance
    @param      dataset Dataset to manage

    It is assumed that @a dataset is already layed out per this class'
    conventions. Typically, it is the result of calling
    create_raster_view(), or similar.
*/
template<
    typename DatasetPtr>
RasterView<DatasetPtr>::RasterView(
    DatasetPtr dataset_ptr,
    std::string const& phenomenon_name,
    std::string const& property_set_name):

    data_model::RasterView<DatasetPtr>{std::move(dataset_ptr), phenomenon_name, property_set_name}

{
    // FIXME Obtain these from base class?
    Dataset& dataset{**this};
    auto& phenomenon{dataset.phenomena()[phenomenon_name]};
    auto& property_set{phenomenon.property_sets()[property_set_name]};

    auto& properties{property_set.properties()};
    auto& space_discretization_property =
        properties.collection<same_shape::Properties>()[space_discretization_property_name];

    // Scan for properties that represent the rasters
    {
        using RasterProperties = different_shape::Properties;
        using RasterProperty = different_shape::Property;

        RasterProperties& raster_properties{properties.collection<RasterProperties>()};

        for(auto const& name: raster_properties.names())
        {
            RasterProperty& property{raster_properties[name]};

            // Perform some sanity checks on the properties to skip the
            // ones that actually aren't raster properties
            bool const property_is_discretized{
                property.space_is_discretized() &&
                property.space_discretization_type() == SpaceDiscretization::regular_grid};

            if(property_is_discretized)
            {
                bool const space_discretization_properties_match{
                    property.space_discretization_property().id() == space_discretization_property.id()};

                if(space_discretization_properties_match)
                {
                    data_model::RasterView<DatasetPtr>::add_layer(name);
                }
            }
        }
    }
}


template<
    typename DatasetPtr>
typename RasterView<DatasetPtr>::Layer RasterView<DatasetPtr>::add_layer(
    std::string const& name,
    hdf5::Datatype const& datatype)
{
    if(this->contains(name)) {
        throw std::runtime_error(
            fmt::format("Raster layer {} already exists", name));
    }

    // FIXME Obtain these from base class?
    Dataset& dataset{**this};
    auto& phenomenon{dataset.phenomena()[this->phenomenon_name()]};
    auto& property_set{phenomenon.property_sets()[this->property_set_name()]};
    auto& properties{property_set.properties()};

    auto& space_discretization_property{
        properties.template collection<same_shape::Properties>()[space_discretization_property_name]};

    using RasterProperties = different_shape::Properties;
    using RasterProperty = different_shape::Property;

    RasterProperties& raster_properties{properties.template collection<RasterProperties>()};

    assert(!raster_properties.contains(name));

    RasterProperty& raster_property{raster_properties.add(name, datatype, 2)};

    raster_property.value().expand(this->object_id(), this->grid_shape());

    raster_property.set_space_discretization(
        SpaceDiscretization::regular_grid, space_discretization_property);

    data_model::RasterView<DatasetPtr>::add_layer(name);

    assert(this->contains(name));

    return raster_property.value()[this->object_id()];
}


template<
    typename DatasetPtr>
typename RasterView<DatasetPtr>::Layer RasterView<DatasetPtr>::layer(
    std::string const& name)
{
    if(!this->contains(name)) {
        throw std::runtime_error(
            fmt::format("Raster layer {} does not exist", name));
    }

    Dataset& dataset{**this};
    auto& phenomenon{dataset.phenomena()[this->phenomenon_name()]};
    auto& property_set{phenomenon.property_sets()[this->property_set_name()]};
    auto& properties{property_set.properties()};

    using RasterProperties = different_shape::Properties;

    RasterProperties& raster_properties{
        properties.template collection<RasterProperties>()};

    assert(raster_properties.contains(name));

    PropertyT<RasterProperties>& raster_property{raster_properties[name]};
    ValueT<RasterProperties>& raster_value{raster_property.value()};

    assert(raster_value.contains(this->object_id()));

    return raster_value[this->object_id()];
}


bool contains_raster(
    Dataset const& dataset,
    std::string const& phenomenon_name,
    std::string const& property_set_name)
{
    bool result{false};

    if(data_model::contains_raster(dataset, phenomenon_name, property_set_name))
    {
        auto const& phenomenon{dataset.phenomena()[phenomenon_name]};
        auto const& property_set{phenomenon.property_sets()[property_set_name]};
        auto const& properties{property_set.properties()};

        if(!properties.contains(time_discretization_property_name))
        {
            result = true;
        }
    }

    return result;
}


/*!
    @brief      Create a RasterView instance
    @param      .
    @return     .
    @exception  .
*/
template<
    typename DatasetPtr>
RasterView<DatasetPtr> create_raster_view(
    DatasetPtr dataset_ptr,
    std::string const& phenomenon_name,
    std::string const& property_set_name,
    hdf5::Shape const& grid_shape,
    typename data_model::RasterView<DatasetPtr>::SpaceBox const& space_box)
{
    // FIXME Refactor with variable::create_raster_view

    Dataset& dataset{*dataset_ptr};

    // Add one phenomenon. It is OK if the phenomenon already exists.
    auto& phenomenon = dataset.phenomena().contains(phenomenon_name)
        ? dataset.phenomena()[phenomenon_name]
        : dataset.add_phenomenon(phenomenon_name);

    // Add the ID of the only object. It is OK if this ID already exists.
    ID object_id{5};
    if(phenomenon.object_id().nr_objects() == 0) {
        phenomenon.object_id().expand(1);
        phenomenon.object_id().write(0, &object_id);
    }

    SpaceConfiguration space_configuration{
        Mobility::stationary,
        SpaceDomainItemType::box};

    static_assert(
        std::is_same_v<
            typename RasterView<DatasetPtr>::SpaceBox::value_type,
            double>);

    auto& property_set = phenomenon.property_sets().add(
        property_set_name,
        space_configuration, hdf5::native_float64, 2);

    // Space domain
    auto space_domain_value{
        property_set.space_domain().value<StationarySpaceBox>()};
    space_domain_value.expand(1);
    space_domain_value.write(0, space_box.data());

    // Discretization properties
    Properties& properties{property_set.properties()};

    hdf5::Datatype const shape_datatype{
        hdf5::NativeDatatypeTraits<hdf5::Shape::value_type>::type_id()};
        // hdf5::native_datatype<hdf5::Shape::value_type>()};

    {
        // Add property for storing space discretization
        auto& property = properties.add<same_shape::Property>(
            space_discretization_property_name, shape_datatype,
            lue::hdf5::Shape{2});  // nr_rows, nr_cols
        auto& value = property.value();
        value.expand(1);
        value.write(0, grid_shape.data());
    }

    return RasterView<DatasetPtr>{std::move(dataset_ptr), phenomenon_name, property_set_name};
}

}  // namespace constant


namespace variable {

/*!
    @brief      Construct an instance
    @param      dataset Dataset to manage

    It is assumed that @a dataset is already layed out per this class'
    conventions. Typically, it is the result of calling
    create_raster_view(), or similar.
*/
template<
    typename DatasetPtr>
RasterView<DatasetPtr>::RasterView(
    DatasetPtr dataset_ptr,
    std::string const& phenomenon_name,
    std::string const& property_set_name):

    data_model::RasterView<DatasetPtr>{
        std::move(dataset_ptr), phenomenon_name, property_set_name},
    _time_box(),
    _time_grid(1)

{
    // FIXME Obtain these from base class?
    Dataset& dataset{**this};
    auto& phenomenon{dataset.phenomena()[phenomenon_name]};
    auto& property_set{phenomenon.property_sets()[property_set_name]};

    // Read time box from domain
    {
        auto& time_domain{property_set.time_domain()};
        auto value{time_domain.value<data_model::TimeBox>()};
        value.read(0, _time_box.data());
    }

    auto& properties{property_set.properties()};

    // Open discretization property and read information
    auto& time_discretization_property =
        properties.collection<same_shape::constant_shape::Properties>()[
            time_discretization_property_name];
    time_discretization_property.value().read(0, _time_grid.data());

    auto& space_discretization_property =
        properties.collection<same_shape::Properties>()[
            space_discretization_property_name];

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

                    data_model::RasterView<DatasetPtr>::add_layer(name);
                }
            }
        }
    }
}


template<
    typename DatasetPtr>
typename RasterView<DatasetPtr>::TimeBox const&
    RasterView<DatasetPtr>::time_box() const
{
    return _time_box;
}


template<
    typename DatasetPtr>
hdf5::Shape::value_type RasterView<DatasetPtr>::nr_time_steps() const
{
    return _time_grid[0];
}


template<
    typename DatasetPtr>
typename RasterView<DatasetPtr>::Layer RasterView<DatasetPtr>::add_layer(
    std::string const& name,
    hdf5::Datatype const& datatype)
{
    if(this->contains(name)) {
        throw std::runtime_error(
            fmt::format("Raster layer {} already exists", name));
    }

    // FIXME Obtain these from base class?
    Dataset& dataset{**this};
    auto& phenomenon{dataset.phenomena()[this->phenomenon_name()]};
    auto& property_set{phenomenon.property_sets()[this->property_set_name()]};
    auto& properties{property_set.properties()};

    auto& time_discretization_property =
        properties.template collection<same_shape::constant_shape::Properties>()[
            time_discretization_property_name];
    auto& space_discretization_property =
        properties.template collection<same_shape::Properties>()[
            space_discretization_property_name];

    using RasterProperties = different_shape::constant_shape::Properties;
    using RasterProperty = different_shape::constant_shape::Property;

    RasterProperties& raster_properties{
        properties.template collection<RasterProperties>()};

    assert(!raster_properties.contains(name));

    RasterProperty& raster_property{raster_properties.add(name, datatype, 3)};

    hdf5::Shape object_array_shape(1 + this->grid_shape().size());
    object_array_shape[0] = nr_time_steps();
    std::copy(
        this->grid_shape().begin(), this->grid_shape().end(),
        object_array_shape.begin() + 1);
    Count const nr_locations_in_time{1};

    Layer layer{raster_property.value().expand(
        this->object_id(), object_array_shape, nr_locations_in_time)};

    raster_property.set_time_discretization(
        TimeDiscretization::regular_grid, time_discretization_property);
    raster_property.set_space_discretization(
        SpaceDiscretization::regular_grid, space_discretization_property);

    data_model::RasterView<DatasetPtr>::add_layer(name);

    assert(this->contains(name));

    return layer;
}


template<
    typename DatasetPtr>
typename RasterView<DatasetPtr>::Layer RasterView<DatasetPtr>::layer(
    std::string const& name)
{
    if(!this->contains(name)) {
        throw std::runtime_error(
            fmt::format("Raster layer {} does not exist", name));
    }

    Dataset& dataset{**this};
    auto& phenomenon{dataset.phenomena()[this->phenomenon_name()]};
    auto& property_set{phenomenon.property_sets()[this->property_set_name()]};
    auto& properties{property_set.properties()};

    using RasterProperties = different_shape::constant_shape::Properties;

    RasterProperties& raster_properties{
        properties.template collection<RasterProperties>()};

    assert(raster_properties.contains(name));

    PropertyT<RasterProperties>& raster_property{raster_properties[name]};
    ValueT<RasterProperties>& raster_value{raster_property.value()};

    assert(raster_value.exists(this->object_id()));

    return raster_value[this->object_id()];
}


bool contains_raster(
    Dataset const& dataset,
    std::string const& phenomenon_name,
    std::string const& property_set_name)
{
    bool result{false};

    if(data_model::contains_raster(dataset, phenomenon_name, property_set_name))
    {
        auto const& phenomenon{dataset.phenomena()[phenomenon_name]};
        auto const& property_set{phenomenon.property_sets()[property_set_name]};

        bool time_domain_ok{false};
        bool time_discretization_ok{false};

        // Time domain
        {
            if(property_set.has_time_domain())
            {
                auto const& time_domain{property_set.time_domain()};
                auto const& configuration{time_domain.configuration()};

                if(configuration.value<TimeDomainItemType>() ==
                        TimeDomainItemType::box)
                {
                    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
                    auto time_box = const_cast<TimeDomain&>(time_domain)
                        .value<TimeBox>();

                    if(time_box.nr_boxes() == 1)
                    {
                        time_domain_ok = true;
                    }
                }
            }
        }

        // Time discretization property
        if(time_domain_ok)
        {
            auto const& properties{property_set.properties()};

            if(properties.contains(time_discretization_property_name))
            {
                if( properties.shape_per_object(
                            time_discretization_property_name) ==
                        ShapePerObject::same &&
                    properties.value_variability(
                            time_discretization_property_name) ==
                        ValueVariability::variable &&
                    properties.shape_variability(
                            time_discretization_property_name) ==
                        ShapeVariability::constant)
                {
                    assert(
                        properties.collection<same_shape::constant_shape::Properties>().contains(
                            time_discretization_property_name));

                    auto const& discretization_property{
                        properties.collection<same_shape::constant_shape::Properties>()[
                            time_discretization_property_name]};
                    auto const& value{discretization_property.value()};

                    if(!discretization_property.time_is_discretized() &&
                        !discretization_property.time_is_discretized() &&
                        value.nr_arrays() == 1 &&
                        value.array_shape() == hdf5::Shape{1} &&
                        value.file_datatype() == hdf5::Datatype{
                            hdf5::StandardDatatypeTraits<
                                hdf5::Shape::value_type>::type_id()})
                        // value.file_datatype() ==
                        //     hdf5::std_datatype_le<hdf5::Shape::value_type>())
                    {
                        time_discretization_ok = true;
                    }
                }
            }
        }

        result = time_domain_ok && time_discretization_ok;
    }

    return result;
}


/*!
    @brief      Create a RasterView instance
    @param      .
    @return     .
    @exception  .
*/
template<
    typename DatasetPtr>
RasterView<DatasetPtr> create_raster_view(
    DatasetPtr dataset_ptr,
    std::string const& phenomenon_name,
    std::string const& property_set_name,
    Clock const& clock,
    Count nr_time_steps,
    typename RasterView<DatasetPtr>::TimeBox const& time_box,
    hdf5::Shape const& grid_shape,
    typename data_model::RasterView<DatasetPtr>::SpaceBox const& space_box)
{
    // FIXME Refactor with constant::create_raster_view

    Dataset& dataset{*dataset_ptr};

    // Add one phenomenon. It is OK if the phenomenon already exists.
    auto& phenomenon = dataset.phenomena().contains(phenomenon_name)
        ? dataset.phenomena()[phenomenon_name]
        : dataset.add_phenomenon(phenomenon_name);

    // Add the ID of the only object. It is OK if this ID already exists.
    ID object_id{5};
    if(phenomenon.object_id().nr_objects() == 0) {
        phenomenon.object_id().expand(1);
        phenomenon.object_id().write(0, &object_id);
    }

    TimeConfiguration time_configuration{
            TimeDomainItemType::box
        };
    SpaceConfiguration space_configuration{
            Mobility::stationary,
            SpaceDomainItemType::box
        };

    static_assert(std::is_same_v<typename RasterView<DatasetPtr>::SpaceBox::value_type, double>);
    auto& property_set = phenomenon.property_sets().add(
        property_set_name, time_configuration, clock,
        space_configuration, hdf5::native_float64, 2);

    // Object tracker
    {
        auto& object_tracker{property_set.object_tracker()};

        Index active_set_idx{0};
        object_tracker.active_set_index().expand(1);
        object_tracker.active_set_index().write(0, &active_set_idx);

        Index active_object_idx{0};
        object_tracker.active_object_index().expand(1);
        object_tracker.active_object_index().write(0, &active_object_idx);

        object_tracker.active_object_id().expand(1);
        object_tracker.active_object_id().write(0, &object_id);
    }

    // Time domain
    auto time_domain_value{property_set.time_domain().value<data_model::TimeBox>()};
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
        auto& property = properties.add<same_shape::constant_shape::Property>(
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
        value.write(0, grid_shape.data());
    }

    return RasterView<DatasetPtr>{
        std::move(dataset_ptr), phenomenon_name, property_set_name};
}

}  // namespace variable


#define INSTANTIATE_RASTER_VIEW(DatasetPtr)                           \
template class RasterView<DatasetPtr>;                                \
template class constant::RasterView<DatasetPtr>;                      \
template class variable::RasterView<DatasetPtr>;                      \
                                                                      \
template constant::RasterView<DatasetPtr>                             \
    constant::create_raster_view<DatasetPtr>(                         \
        DatasetPtr dataset,                                           \
        std::string const& phenomenon_name,                           \
        std::string const& property_set_name,                         \
        hdf5::Shape const& grid_shape,                                \
        typename data_model::RasterView<DatasetPtr>::SpaceBox const&  \
            space_box);                                               \
                                                                      \
template variable::RasterView<DatasetPtr>                             \
    variable::create_raster_view<DatasetPtr>(                         \
        DatasetPtr dataset,                                           \
        std::string const& phenomenon_name,                           \
        std::string const& property_set_name,                         \
        Clock const& clock,                                           \
        Count nr_time_steps,                                          \
        typename RasterView<DatasetPtr>::TimeBox const& time_box,     \
        hdf5::Shape const& grid_shape,                                \
        typename data_model::RasterView<DatasetPtr>::SpaceBox const&  \
            space_box);

INSTANTIATE_RASTER_VIEW(Dataset*)
INSTANTIATE_RASTER_VIEW(std::shared_ptr<Dataset>)

#undef INSTANTIATE_DATASET_VIEW

}  // namespace data_model
}  // namespace lue

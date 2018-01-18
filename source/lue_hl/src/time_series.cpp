#include "lue/hl/time_series.hpp"
#include "lue/constant_size/time/located/time_box_domain.hpp"
#include "lue/constant_size/time/omnipresent/property_set.hpp"
#include "lue/constant_size/time/omnipresent/same_shape/property.hpp"
#include "lue/constant_size/time/omnipresent/space_point_domain.hpp"
#include <numeric>


namespace lue {
namespace hl {
namespace {

std::string const space_point_property_set_name = "space_point";
std::string const time_discretization_phenomenon_name = "time_boxes";
std::string const time_discretization_property_set_name = "discretization";
std::string const time_discretization_property_name = "nr_steps";

}  // Anonymous namespace

namespace omnipresent = constant_size::time::omnipresent;


TimeSeries::SpaceDomain::SpaceDomain()

    : _crs{},
      _coordinates{}

{
}


TimeSeries::SpaceDomain::SpaceDomain(
    std::string const& crs,
    Coordinates&& coordinates)

    : _crs{crs},
      _coordinates{std::forward<Coordinates>(coordinates)}

{
    assert(_coordinates.size() % 2 == 0);
}


std::string const& TimeSeries::SpaceDomain::crs() const
{
    return _crs;
}


TimeSeries::SpaceDomain::Coordinates const& TimeSeries::SpaceDomain::
    coordinates() const
{
    return _coordinates;
}


std::size_t TimeSeries::SpaceDomain::nr_points() const
{
    assert(_coordinates.size() % 2 == 0);
    return _coordinates.size() / 2;
}


TimeSeries::TimeSeries(
    hdf5::Identifier const& phenomenon_id,
    std::string const& property_set_name,
    std::string const& property_name)

    : _phenomenon{hdf5::Group{phenomenon_id}},
      _property_set{
          hdf5::Group{_phenomenon.property_sets()[property_set_name].id()}},
      _value_property{
          hdf5::Group{_property_set.properties()[property_name].id()},
          hdf5::Datatype{H5T_NATIVE_DOUBLE}},
      _time_domain{},
      _space_domain{},
      _time_discretization{}

{
    // Time box domain
    {
        located::TimeBoxDomain time_box_domain{_property_set.domain()};
        TimeSeriesDomain::Coordinates coordinates;
        time_box_domain.items().read(coordinates.data());
        _time_domain = TimeSeriesDomain{
            time_box_domain.configuration().clock(),
                coordinates[0], coordinates[1]};
    }

    // Space point domain
    {
        auto const& property_set =
            _phenomenon.property_sets()[space_point_property_set_name];
        omnipresent::SpacePointDomain space_point_domain{
            property_set.domain(),
            hdf5::Datatype{hdf5::NativeDatatypeTraits<double>::type_id()}};

        auto const& space_points = space_point_domain.items();
        hl::TimeSeries::SpaceDomain::Coordinates coordinates(
            2 * space_points.nr_items());
        space_points.read(coordinates.data());
        _space_domain = TimeSeries::SpaceDomain{"blah", std::move(coordinates)};
    }

    // Time discretization
    {
        assert(_value_property.time_is_discretized());
        // This property is linked from the value property
        auto discretization_property = omnipresent::same_shape::Property{
            hdf5::Group{_value_property.time_discretization().id()},
            hdf5::Datatype{hdf5::NativeDatatypeTraits<hsize_t>::type_id()}
        };
        assert(discretization_property.values().nr_items() == 1);
        hsize_t nr_steps;
        discretization_property.values().read(&nr_steps);
        _time_discretization = TimeSeriesDiscretization{nr_steps};
    }
}


std::string TimeSeries::name() const
{
    // Name of the time series is the name of the property containing
    // the values
    return _value_property.id().name();
}


hsize_t TimeSeries::nr_items() const
{
    return _property_set.ids().nr_items();
}


TimeSeriesDomain const& TimeSeries::time_domain() const
{
    return _time_domain;
}


TimeSeriesDiscretization const& TimeSeries::time_discretization() const
{
    return _time_discretization;
}


TimeSeries::SpaceDomain const& TimeSeries::space_domain() const
{
    return _space_domain;
}


hdf5::Hyperslab TimeSeries::hyperslab(
    std::size_t const item_idx) const
{
    // The underlying dataset for storing time series values for all items
    // is organized as folows:
    // [nr_time_domain_items, nr_items, value_shape]
    auto const nr_time_steps = _time_discretization.nr_steps();
    auto const nr_items = this->nr_items();

    hdf5::Offset offset{0, item_idx};
    hdf5::Stride stride{1, nr_items};
    hdf5::Count count{nr_time_steps, 1u};
    assert(offset.size() == 2);
    assert(stride.size() == 2);
    assert(count.size() == 2);

    return hdf5::Hyperslab{offset, stride, count};
}


void TimeSeries::write(
    std::size_t const item_idx,
    double const* values)
{
    auto const nr_time_steps = _time_discretization.nr_steps();
    auto const nr_items = this->nr_items();

    auto& values_ = _value_property.values();
    values_.reserve(nr_time_steps, nr_items);
    values_.write(hyperslab(item_idx), values);
}


void TimeSeries::read(
    std::size_t const item_idx,
    double* values)
{
    _value_property.values().read(hyperslab(item_idx), values);
}


namespace {

TimeSeries create_time_series(
    Phenomenon& phenomenon,
    Phenomenon& time_discretization_phenomenon,
    std::string const& property_set_name,
    TimeSeriesDomain const& time_domain,
    TimeSeries::SpaceDomain const& space_domain,
    std::string const& property_name,
    TimeSeriesDiscretization const& time_discretization)
{
    if(!phenomenon.property_sets().contains(property_set_name)) {

        std::size_t const nr_boxes = 1;

        // Each space point from the time series represents an item
        hsize_t const nr_items = space_domain.nr_points();

        // Create property-set for time series values
        auto property_set = located::create_property_set(
            phenomenon.property_sets(), property_set_name);

        {
            // Write item ids to property set
            {
                auto ids_ = std::make_unique<hsize_t[]>(nr_items);
                std::iota(ids_.get(), ids_.get() + nr_items, 0);
                auto& ids = property_set.ids();
                ids.reserve(nr_items);
                ids.write(ids_.get());
            }

            // Create time-box domain
            {
                auto time_box_domain = located::create_time_box_domain(
                    property_set, time_domain.clock());
                auto& time_boxes = time_box_domain.reserve(nr_boxes);
                time_boxes.write(time_domain.coordinates().data());
            }
        }

        // Create property set for storing space points. These don't change over
        // time, so they need to be stored in a different property set than the
        // one with the time series values.
        {
            auto const& ids = property_set.ids();
            auto property_set = omnipresent::create_property_set(
                phenomenon, space_point_property_set_name, ids);
            auto const file_datatype_id =
                hdf5::Datatype{hdf5::StandardDatatypeTraits<double>::type_id()};
            auto const memory_datatype_id =
                hdf5::Datatype{hdf5::NativeDatatypeTraits<double>::type_id()};
            size_t const rank = 2;
            auto space_domain_ = omnipresent::create_space_point_domain(
                property_set, file_datatype_id, memory_datatype_id, rank);
            space_domain_.reserve(nr_items).write(
                space_domain.coordinates().data());
        }


        // Create property set for storing time discretization
        // information. This information is shared by all items,
        // and doesn't change over time.
        {
            auto discretization_property_set = omnipresent::create_property_set(
                time_discretization_phenomenon,
                time_discretization_property_set_name);

            // For each time box a value representing the number of steps
            // used to discretize the box
            hsize_t const nr_items = nr_boxes;

            // Write item ids to property set
            {
                auto ids_ = std::make_unique<hsize_t[]>(nr_items);
                std::iota(ids_.get(), ids_.get() + nr_items, 0);
                auto& ids = discretization_property_set.ids();
                ids.reserve(nr_items);
                ids.write(ids_.get());
            }

            // Add property
            hdf5::Shape const shape{1};
            auto const file_datatype_id =
                hdf5::Datatype{hdf5::StandardDatatypeTraits<hsize_t>::type_id()};
            auto const memory_datatype_id =
                hdf5::Datatype{hdf5::NativeDatatypeTraits<hsize_t>::type_id()};
            auto discretization_property =
                omnipresent::same_shape::create_property(
                    discretization_property_set,
                    time_discretization_property_name,
                    file_datatype_id,
                    memory_datatype_id,
                    shape);
            discretization_property.values().reserve(nr_items);
            discretization_property.values().write(
                time_discretization.shape().data());

            // Create property for storing time series values
            {
                auto const file_datatype_id =
                    hdf5::Datatype{hdf5::StandardDatatypeTraits<double>
                        ::type_id()};
                auto const memory_datatype_id =
                    hdf5::Datatype{hdf5::NativeDatatypeTraits<double>
                        ::type_id()};
                auto value_property =
                    located::shared::constant_shape::same_shape::create_property(
                        property_set,
                        property_name,
                        file_datatype_id,
                        memory_datatype_id);

                value_property.discretize_time(discretization_property);
            }
        }
    }

    return TimeSeries{phenomenon.id(), property_set_name, property_name};
}

}  // Anonymous namespace


/*!
    @brief      Add representation of a time series to @a dataset
    @param      dataset Dataset to add time series to
    @param      phenomenon_name Name of phenomenon to add time series to
    @param      property_set_name Name of property set to add time series to
    @param      time_domain Time domain
    @param      space_domain Space domain
    @param      property_name Name of property for time series values
    @param      time_discretization Time discretization of time series value

    If no phenomenon with the name passed in exists in @a dataset,
    it will be added to the dataset.
*/
TimeSeries create_time_series(
    Dataset& dataset,
    std::string const& phenomenon_name,
    std::string const& property_set_name,
    TimeSeriesDomain const& time_domain,
    TimeSeries::SpaceDomain const& space_domain,
    std::string const& property_name,
    TimeSeriesDiscretization const& time_discretization)
{
    assert(property_set_name != time_discretization_property_name);

    if(dataset.phenomena().contains(phenomenon_name)) {
        throw std::runtime_error(
            "Importing timeseries in existing property-set is not supported "
            "yet");
    }

    Phenomenon& phenomenon = !dataset.phenomena().contains(phenomenon_name)
        ? dataset.add_phenomenon(phenomenon_name)
        : dataset.phenomena()[phenomenon_name]
        ;

    return create_time_series(
        phenomenon,
        // This can also be a phenomenon with a temp name. As long as one
        // with that name doesn't already exist.
        dataset.add_phenomenon(time_discretization_phenomenon_name),
        property_set_name, time_domain, space_domain,
        property_name, time_discretization);
}

}  // namespace hl
}  // namespace lue

#include "lue/translate/format/json.hpp"
#include "lue/lue.hpp"
#include <nlohmann/json.hpp>


using json = nlohmann::json;


namespace lue {
namespace utility {
namespace {

void throw_missing_entry(
    std::string const& pathname,
    std::string const& location,
    std::string const& entry_name)
{
    throw std::runtime_error(fmt::format(
        "Expected JSON entry '{}' at {} in {}",
        entry_name, location, pathname));
}


bool contains(
    ::json const& json,
    std::string const& name)
{
    return json.find(name) != json.end();
}


void add_object_tracker(
    ::json const& object_tracker_json,
    ObjectTracker& object_tracker)
{
    if(contains(object_tracker_json, "active_set_index")) {
        std::vector<Index> const active_set_idx =
            object_tracker_json.at("active_set_index");
        object_tracker.active_set_index().expand(active_set_idx.size());
        object_tracker.active_set_index().write(active_set_idx.data());
    }

    if(contains(object_tracker_json, "active_object_id")) {
        std::vector<ID> const active_object_id =
            object_tracker_json.at("active_object_id");
        object_tracker.active_object_id().expand(active_object_id.size());
        object_tracker.active_object_id().write(active_object_id.data());
    }
}


void add_time_points(
    ::json const& time_point_json,
    TimeDomain& time_domain)
{
    std::vector<time::DurationCount> const time_points = time_point_json;
    auto value = time_domain.value<TimePoint>();

    value.expand(time_points.size());
    value.write(time_points.data());
}


template<
    typename Property>
void               add_property        (::json const& property_json,
                                        PropertySet& property_set);


template<>
void add_property<same_shape::Property>(
    ::json const& property_json,
    PropertySet& property_set)
{
    std::string const name = property_json.at("name");

    std::string const datatype_json = property_json.at("datatype");
    assert(datatype_json == "uint32");
    using Datatype = std::uint32_t;
    auto const datatype = hdf5::Datatype{
        hdf5::NativeDatatypeTraits<Datatype>::type_id()};

    using Property = same_shape::Property;
    auto& property = property_set.properties().add<Property>(
        name, datatype);

    std::vector<Datatype> const values = property_json.at("value");
    property.value().expand(values.size());
    property.value().write(values.data());
}


template<>
void add_property<same_shape::constant_shape::Property>(
    ::json const& property_json,
    PropertySet& property_set)
{
    std::string const name = property_json.at("name");

    std::string const shape_variability_json =
        property_json.at("shape_variability");
    auto const shape_variability =
        string_to_aspect<ShapeVariability>(shape_variability_json);
    assert(shape_variability == ShapeVariability::constant);

    std::string const datatype_json = property_json.at("datatype");
    assert(datatype_json == "uint64");
    using Datatype = std::uint64_t;
    auto const datatype = hdf5::Datatype{
        hdf5::NativeDatatypeTraits<Datatype>::type_id()};

    using Property = same_shape::constant_shape::Property;
    auto& property = property_set.properties().add<Property>(
        name, datatype);

    std::vector<Datatype> const values = property_json.at("value");
    property.value().expand(values.size());
    property.value().write(values.data());
}


void add_property(
    ::json const& property_json,
    PropertySet& property_set)
{
    // std::string const description = property_json["description"];

    std::string const value_variability_json =
        property_json.at("value_variability");
    auto const value_variability =
        string_to_aspect<ValueVariability>(value_variability_json);

    std::string const shape_per_object_json =
        property_json.at("shape_per_object");
    auto const shape_per_object =
        string_to_aspect<ShapePerObject>(shape_per_object_json);
    assert(shape_per_object == ShapePerObject::same);

    switch(value_variability) {
        case ValueVariability::constant: {
            add_property<same_shape::Property>(property_json, property_set);
            break;
        }
        case ValueVariability::variable: {
            add_property<same_shape::constant_shape::Property>(
                property_json, property_set);
            break;
        }
    }
}


void add_property_set(
    ::json const& property_set_json,
    Phenomenon& phenomenon)
{
    std::string const name = property_set_json.at("name");

    using PropertySetRef = std::reference_wrapper<PropertySet>;
    using OptionalPropertySet = std::optional<PropertySetRef>;

    OptionalPropertySet property_set_ref;

    if(!contains(property_set_json, "time_domain")) {
        auto& property_set = phenomenon.property_sets().add(name);
        property_set_ref = property_set;
    }
    else {
        auto const& time_domain_json = property_set_json.at("time_domain");
        auto const& clock_json = time_domain_json.at("clock");

        std::string const epoch = clock_json.at("epoch");
        time::DurationCount const tick_period_count =
            clock_json.at("tick_period_count");
        std::string const unit = clock_json.at("unit");

        // TODO Read the time domain item type from the json
        TimeConfiguration const time_configuration{
                TimeDomainItemType::point
            };
        Clock const clock{
                string_to_aspect<time::Unit>(unit),
                tick_period_count
            };
        auto& property_set = phenomenon.property_sets().add(
            name, time_configuration, clock);
        property_set_ref = property_set;

        add_object_tracker(
            property_set_json.at("object_tracker"),
            property_set.object_tracker());
        add_time_points(
            time_domain_json.at("time_point"), property_set.time_domain());
    }

    assert(property_set_ref);

    if(property_set_ref) {
        auto& property_set = *property_set_ref;

        if(contains(property_set_json, "properties")) {
            for(auto const& property_json:
                    property_set_json.at("properties")) {
                add_property(property_json, property_set);
            }
        }
    }
}


void add_phenomenon(
    ::json const& phenomenon_json,
    Phenomena& phenomena)
{
    std::string const name = phenomenon_json.at("name");
    auto& phenomenon = phenomena.add(name);

    if(contains(phenomenon_json, "object_id")) {
        std::vector<Index> const object_id = phenomenon_json.at("object_id");
        phenomenon.object_id().expand(object_id.size());
        phenomenon.object_id().write(object_id.data());
    }

    if(contains(phenomenon_json, "property_sets")) {
        for(auto const& property_set_json:
                phenomenon_json.at("property_sets")) {
            add_property_set(property_set_json, phenomenon);
        }
    }
}


void add_universe(
    ::json const& universe_json,
    Universes& universes)
{
    std::string const name = universe_json.at("name");
    auto& universe = universes.add(name);

    if(contains(universe_json, "phenomena")) {
        for(auto const& phenomenon_json: universe_json.at("phenomena")) {
            add_phenomenon(phenomenon_json, universe.phenomena());
        }
    }
}

}  // Anonymous namespace


void translate_json_to_lue(
    std::string const& json_pathname,
    std::string const& lue_pathname,
    Metadata const& /* metadata */)
{
    // For now, assume we need to create a new dataset, based on the input

    std::ifstream stream(json_pathname);

    if(!stream) {
        throw std::runtime_error("Cannot open " + json_pathname);
    }

    ::json lue_json;
    stream >> lue_json;

    if(!contains(lue_json, "dataset")) {
        throw_missing_entry(json_pathname, "root", "dataset");
    }

    auto dataset = create_dataset(lue_pathname);
    auto const& dataset_json = lue_json.at("dataset");

    if(contains(dataset_json, "universes")) {
        for(auto const& universe_json: dataset_json.at("universes")) {
            add_universe(universe_json, dataset.universes());
        }
    }

    if(contains(dataset_json, "phenomena")) {
        for(auto const& phenomenon_json: dataset_json.at("phenomena")) {
            add_phenomenon(phenomenon_json, dataset.phenomena());
        }
    }
}

}  // namespace utility
}  // namespace lue

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
        std::vector<Index> active_set_idx_json =
            object_tracker_json.at("active_set_index");
        auto& active_set_idx = object_tracker.active_set_index();

        // When appending active sets, we have to update the indices
        // read from the JSON. This also works when no active sets are
        // stored already.
        Index offset = object_tracker.active_object_id().nr_ids();
        std::transform(
            active_set_idx_json.begin(), active_set_idx_json.end(),
            active_set_idx_json.begin(), [offset](auto const idx) {
                    return idx + offset;
                }
            );

        Index const begin = active_set_idx.nr_indices();
        IndexRange const index_range{
            begin, begin + active_set_idx_json.size()};
        active_set_idx.expand(active_set_idx_json.size());
        active_set_idx.write(index_range, active_set_idx_json.data());
    }

    if(contains(object_tracker_json, "active_object_id")) {
        std::vector<ID> const active_object_id_json =
            object_tracker_json.at("active_object_id");
        auto& active_object_id = object_tracker.active_object_id();

        Index const begin = active_object_id.nr_ids();
        IndexRange const index_range{
            begin, begin + active_object_id_json.size()};
        active_object_id.expand(active_object_id_json.size());
        active_object_id.write(index_range, active_object_id_json.data());
    }
}


void add_time_points(
    ::json const& time_point_json,
    TimeDomain& time_domain)
{
    std::vector<time::DurationCount> const time_points = time_point_json;
    auto value = time_domain.value<TimePoint>();

    Index const begin = value.nr_points();
    IndexRange const index_range{begin, begin + time_points.size()};
    value.expand(time_points.size());
    value.write(index_range, time_points.data());
}


template<
    typename Datatype>
void add_same_shape_property(
    ::json const& property_json,
    same_shape::Properties& properties)
{
    std::string const name = property_json.at("name");

    hdf5::Shape shape;

    if(contains(property_json, "shape")) {
        shape = property_json.at("shape");
    }

    auto const nr_elements_in_object_array = size_of_shape(shape, 1);

    auto const datatype = hdf5::Datatype{
        hdf5::NativeDatatypeTraits<Datatype>::type_id()};

    std::vector<Datatype> const values = property_json.at("value");

    if(values.size() % nr_elements_in_object_array != 0) {
        throw std::runtime_error(fmt::format(
            "Number of values is not a multiple of the number of elements "
            "in an object array ({} % {} != 0)",
            values.size(), nr_elements_in_object_array));
    }

    auto& property = properties.contains(name)
        ? properties[name]
        : properties.add(name, datatype, shape)
        ;

    Index const begin = property.value().nr_arrays();
    auto const nr_object_arrays =
        values.size() / nr_elements_in_object_array;
    IndexRange const index_range{begin, begin + nr_object_arrays};
    property.value().expand(nr_object_arrays);
    property.value().write(index_range, values.data());


    // TODO
    // datatype, shape must match, etc
    // branch on contains()
}


// template<
//     typename Properties,
//     typename Datatype>
// void               add_property        (::json const& property_json,
//                                         Properties& properties);


// template<>
// void add_property<same_shape::Properties, std::uint32_t>(
//     ::json const& property_json,
//     same_shape::Properties& properties)
// {
//     std::string const name = property_json.at("name");
// 
//     hdf5::Shape shape;
// 
//     if(contains(property_json, "shape")) {
//         shape = property_json.at("shape");
//     }
// 
//     auto const nr_elements_in_object_array = size_of_shape(shape, 1);
// 
//     using Datatype = std::uint32_t;
// 
//     auto const datatype = hdf5::Datatype{
//         hdf5::NativeDatatypeTraits<Datatype>::type_id()};
// 
//     std::vector<Datatype> const values = property_json.at("value");
// 
//     if(values.size() % nr_elements_in_object_array != 0) {
//         throw std::runtime_error(fmt::format(
//             "Number of values is not a multiple of the number of elements "
//             "in an object array ({} % {} != 0)",
//             values.size(), nr_elements_in_object_array));
//     }
// 
//     auto& property = properties.contains(name)
//         ? properties[name]
//         : properties.add(name, datatype, shape)
//         ;
// 
//     Index const begin = property.value().nr_arrays();
//     auto const nr_object_arrays =
//         values.size() / nr_elements_in_object_array;
//     IndexRange const index_range{begin, begin + nr_object_arrays};
//     property.value().expand(nr_object_arrays);
//     property.value().write(index_range, values.data());
// 
// 
//     // TODO
//     // datatype, shape must match, etc
//     // branch on contains()
// 
// }


template<
    typename Datatype>
void add_same_shape_constant_shape_property(
    ::json const& property_json,
    same_shape::constant_shape::Properties& properties)
{
    std::string const name = property_json.at("name");

    std::string const shape_variability_json =
        property_json.at("shape_variability");
    auto const shape_variability =
        string_to_aspect<ShapeVariability>(shape_variability_json);
    assert(shape_variability == ShapeVariability::constant);

    std::string const datatype_json = property_json.at("datatype");
    assert(datatype_json == "uint64");

    auto const datatype = hdf5::Datatype{
        hdf5::NativeDatatypeTraits<Datatype>::type_id()};

    hdf5::Shape shape;
    if(contains(property_json, "shape")) {
        shape = property_json.at("shape");
    }
    auto const nr_elements_in_object_array = size_of_shape(shape, 1);

    auto& property = properties.contains(name)
        ? properties[name]
        : properties.add(name, datatype, shape)
        ;

    // TODO
    // datatype, shape must match, etc
    // branch on contains()

    std::vector<Datatype> const values = property_json.at("value");
    // Index const begin = property.value().nr_arrays();
    // IndexRange const index_range{begin, begin + values.size()};
    // property.value().expand(values.size());
    // property.value().write(index_range, values.data());

    if(values.size() % nr_elements_in_object_array != 0) {
        throw std::runtime_error(fmt::format(
            "Number of values is not a multiple of the number of elements "
            "in an object array ({} % {} != 0)",
            values.size(), nr_elements_in_object_array));
    }

    auto const nr_object_arrays = values.size() / nr_elements_in_object_array;
    Index const begin = property.value().nr_arrays();
    IndexRange const index_range{begin, begin + nr_object_arrays};
    property.value().expand(nr_object_arrays);
    property.value().write(index_range, values.data());
}


// template<>
// void add_property<same_shape::constant_shape::Properties, std::uint32_t>(
//     ::json const& property_json,
//     same_shape::constant_shape::Properties& properties)
// {
//     std::string const name = property_json.at("name");
// 
//     std::string const shape_variability_json =
//         property_json.at("shape_variability");
//     auto const shape_variability =
//         string_to_aspect<ShapeVariability>(shape_variability_json);
//     assert(shape_variability == ShapeVariability::constant);
// 
//     std::string const datatype_json = property_json.at("datatype");
//     assert(datatype_json == "uint64");
//     using Datatype = std::uint64_t;
//     auto const datatype = hdf5::Datatype{
//         hdf5::NativeDatatypeTraits<Datatype>::type_id()};
// 
//     hdf5::Shape shape;
//     if(contains(property_json, "shape")) {
//         shape = property_json.at("shape");
//     }
//     auto const nr_elements_in_object_array = size_of_shape(shape, 1);
// 
//     auto& property = properties.contains(name)
//         ? properties[name]
//         : properties.add(name, datatype, shape)
//         ;
// 
//     // TODO
//     // datatype, shape must match, etc
//     // branch on contains()
// 
//     std::vector<Datatype> const values = property_json.at("value");
//     // Index const begin = property.value().nr_arrays();
//     // IndexRange const index_range{begin, begin + values.size()};
//     // property.value().expand(values.size());
//     // property.value().write(index_range, values.data());
// 
//     if(values.size() % nr_elements_in_object_array != 0) {
//         throw std::runtime_error(fmt::format(
//             "Number of values is not a multiple of the number of elements "
//             "in an object array ({} % {} != 0)",
//             values.size(), nr_elements_in_object_array));
//     }
// 
//     auto const nr_object_arrays = values.size() / nr_elements_in_object_array;
//     Index const begin = property.value().nr_arrays();
//     IndexRange const index_range{begin, begin + nr_object_arrays};
//     property.value().expand(nr_object_arrays);
//     property.value().write(index_range, values.data());
// }


void add_property(
    ::json const& property_json,
    Properties& properties)
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

    std::string const datatype_json = property_json.at("datatype");

    switch(value_variability) {
        case ValueVariability::constant: {
            using Properties = same_shape::Properties;

            if(datatype_json == "uint32") {
                using Datatype = std::uint32_t;

                add_same_shape_property<Datatype>(
                    property_json, properties.collection<Properties>());
            }
            else {
                throw std::runtime_error(fmt::format(
                    "Datatype {} not supported yet",
                    datatype_json));
            }

            break;
        }
        case ValueVariability::variable: {
            using Properties = same_shape::constant_shape::Properties;

            if(datatype_json == "uint32") {
                using Datatype = std::uint32_t;

                add_same_shape_constant_shape_property<Datatype>(
                    property_json, properties.collection<Properties>());
            }
            else {
                throw std::runtime_error(fmt::format(
                    "Datatype {} not supported yet",
                    datatype_json));
            }

            break;
        }
    }
}


void add_property_set(
    ::json const& property_set_json,
    PropertySets& property_sets)
{
    std::string const name = property_set_json.at("name");

    using PropertySetRef = std::reference_wrapper<PropertySet>;
    using OptionalPropertySet = std::optional<PropertySetRef>;

    OptionalPropertySet property_set_ref;

    if(!contains(property_set_json, "time_domain")) {
        auto& property_set = property_sets.contains(name)
            ? property_sets[name]
            : property_sets.add(name)
            ;
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
        // TODO Store epoch!!!
        TimeConfiguration const time_configuration{
                TimeDomainItemType::point
            };
        Clock const clock{
                string_to_aspect<time::Unit>(unit),
                tick_period_count
            };

        if(property_sets.contains(name)) {
            auto& property_set = property_sets[name];
            property_set_ref = property_set;

            if(property_set.time_domain().configuration() !=
                    time_configuration) {
                throw std::runtime_error(fmt::format(
                    "Existing time configuration at {} does not match "
                    "the one in the JSON file",
                    property_set.id().pathname()));
            }

            if(property_set.time_domain().clock() != clock) {
                throw std::runtime_error(fmt::format(
                    "Existing clock at {} does not match "
                    "the one in the JSON file",
                    property_set.id().pathname()));
            }
        }
        else {
            auto& property_set =
                property_sets.add(name, time_configuration, clock);
            property_set_ref = property_set;
        }

        PropertySet& property_set = *property_set_ref;

        add_object_tracker(
            property_set_json.at("object_tracker"),
            property_set.object_tracker());
        add_time_points(
            time_domain_json.at("time_point"), property_set.time_domain());
    }

    assert(property_set_ref);

    if(property_set_ref) {
        PropertySet& property_set = *property_set_ref;

        if(contains(property_set_json, "properties")) {
            for(auto const& property_json:
                    property_set_json.at("properties")) {
                add_property(property_json, property_set.properties());
            }
        }
    }
}


void add_phenomenon(
    ::json const& phenomenon_json,
    Phenomena& phenomena)
{
    std::string const name = phenomenon_json.at("name");
    auto& phenomenon = phenomena.contains(name)
        ? phenomena[name]
        : phenomena.add(name)
        ;

    if(contains(phenomenon_json, "object_id")) {
        // JSON contains IDs of objects having information that doesn't
        // change through time.
        std::vector<Index> const object_id = phenomenon_json.at("object_id");

        // This logic should work when this is the first collection of IDs
        // and when this collection should be appended to an existing
        // collection of IDs
        Index const begin = phenomenon.object_id().nr_arrays();
        IndexRange const index_range{begin, begin + object_id.size()};
        phenomenon.object_id().expand(object_id.size());
        phenomenon.object_id().write(index_range, object_id.data());
    }

    if(contains(phenomenon_json, "collection_property_sets")) {
        for(auto const& property_set_json:
                phenomenon_json.at("collection_property_sets")) {
            add_property_set(
                property_set_json, phenomenon.collection_property_sets());
        }
    }

    if(contains(phenomenon_json, "property_sets")) {
        for(auto const& property_set_json:
                phenomenon_json.at("property_sets")) {
            add_property_set(
                property_set_json, phenomenon.property_sets());
        }
    }
}


void add_universe(
    ::json const& universe_json,
    Universes& universes)
{
    std::string const name = universe_json.at("name");
    auto& universe = universes.contains(name)
        ? universes[name]
        : universes.add(name)
        ;

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
    bool const add,
    Metadata const& /* metadata */)
{
    std::ifstream stream(json_pathname);

    if(!stream) {
        throw std::runtime_error("Cannot open " + json_pathname);
    }

    ::json lue_json;
    stream >> lue_json;

    if(!contains(lue_json, "dataset")) {
        throw_missing_entry(json_pathname, "root", "dataset");
    }

    // Either create a new dataset or add the information stored in the
    // JSON to an existing dataset
    auto dataset = add && dataset_exists(lue_pathname)
        ? Dataset{lue_pathname, H5F_ACC_RDWR}
        : create_dataset(lue_pathname)
        ;
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

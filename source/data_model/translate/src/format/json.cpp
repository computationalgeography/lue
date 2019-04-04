#include "lue/translate/format/json.hpp"
#include "lue/translate/format/gdal_raster.hpp"
#include "lue/data_model.hpp"
#include "lue/navigate.hpp"
#include <nlohmann/json.hpp>
#include <optional>


using json = nlohmann::json;


namespace lue {
namespace utility {
namespace {

void throw_unsupported(
    std::string const& message)
{
    throw std::runtime_error(message);
}


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


template<
    typename Collection>
hdf5::Shape read_gdal_raster(
    std::string const& pathname,
    Collection& collection)
{
    using ValueType = typename Collection::value_type;

    // Open dataset
    GDALRaster raster{pathname};

    // Verify dataset is OK for us
    assert(raster.nr_bands() == 1);
    auto band = raster.band(1);

    assert(band.datatype() == hdf5::native_datatype<ValueType>());

    // Read all cells
    auto const nr_rows = raster.discretization().nr_rows();
    auto const nr_cols = raster.discretization().nr_cols();
    collection.resize(nr_rows * nr_cols);
    band.read(collection.data());

    return hdf5::Shape{nr_rows, nr_cols};
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
    typename SpaceBox,
    typename Coordinate>
void add_space_boxes(
    ::json const& space_box_json,
    SpaceDomain& space_domain)
{
    auto value = space_domain.value<StationarySpaceBox>();
    std::vector<Coordinate> const space_boxes = space_box_json;

    auto const shape = value.array_shape();
    auto const nr_elements_in_object_array = size_of_shape(shape, 1);
    auto const nr_object_arrays =
        space_boxes.size() / nr_elements_in_object_array;

    Index const begin = value.nr_boxes();
    IndexRange const index_range{begin, begin + nr_object_arrays};
    value.expand(nr_object_arrays);
    value.write(index_range, space_boxes.data());
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
    auto const datatype = hdf5::native_datatype<Datatype>();
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


template<
    typename Datatype>
void add_different_shape_property(
    ::json const& property_json,
    different_shape::Properties& properties)
{
    std::string const name = property_json.at("name");
    std::size_t rank = property_json.at("rank");
    auto const datatype = hdf5::native_datatype<Datatype>();
    auto const value_json = property_json.at("value");
    hdf5::Shape shape;

    auto& property = properties.contains(name)
        ? properties[name]
        : properties.add(name, datatype, rank)
        ;

    std::vector<Datatype> values;

    for(auto const& a_value_json: value_json) {
        ID const id = a_value_json.at("id");

        if(contains(a_value_json, "dataset")) {
            // Value is stored in an external dataset
            std::string const dataset_name = a_value_json.at("dataset");
            shape = read_gdal_raster(dataset_name, values);
        }
        else {
            // Value is stored inline
            values = a_value_json.get<decltype(values)>();
            shape = a_value_json.at("shape");
        }

        // Each object has a uniquely shaped value. This value does not
        // change over time, so it cannot be appended to. It must not
        // already exist.
        // assert(!property.value().exists(id));

        property.value().expand(1, &id, &shape);
        property.value()[id].write(values.data());
    }

    if(contains(property_json, "space_discretization")) {
        auto const discretization_json =
            property_json.at("space_discretization");
        auto const discretization_type =
            string_to_aspect<SpaceDiscretization>(
                discretization_json.at("type"));

        std::string const property_path = discretization_json.at("property");

        // Give the current property, navigate to the property pointed
        // to by the path
        auto discretization_property = lue::property(property, property_path);

        property.set_space_discretization(
            discretization_type, discretization_property);
    }
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

    std::string const datatype_json = property_json.at("datatype");

    switch(value_variability) {
        case ValueVariability::constant: {

            switch(shape_per_object) {
                case ShapePerObject::same: {
                    // constant_value / same_shape
                    using Properties = same_shape::Properties;

                    if(datatype_json == "uint32") {
                        using Datatype = std::uint32_t;

                        add_same_shape_property<Datatype>(
                            property_json, properties.collection<Properties>());
                    }
                    else if(datatype_json == "uint64") {
                        using Datatype = std::uint64_t;

                        add_same_shape_property<Datatype>(
                            property_json, properties.collection<Properties>());
                    }
                    else if(datatype_json == "float32") {
                        using Datatype = float;

                        add_same_shape_property<Datatype>(
                            property_json, properties.collection<Properties>());
                    }
                    else if(datatype_json == "float64") {
                        using Datatype = double;

                        add_same_shape_property<Datatype>(
                            property_json, properties.collection<Properties>());
                    }
                    else if(datatype_json == "string") {
                        using Datatype = std::string;

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
                case ShapePerObject::different: {
                    // constant_value / different_shape
                    using Properties = different_shape::Properties;

                    if(datatype_json == "uint32") {
                        using Datatype = std::uint32_t;

                        add_different_shape_property<Datatype>(
                            property_json, properties.collection<Properties>());
                    }
                    else if(datatype_json == "uint64") {
                        using Datatype = std::uint64_t;

                        add_different_shape_property<Datatype>(
                            property_json, properties.collection<Properties>());
                    }
                    else if(datatype_json == "float32") {
                        using Datatype = float;

                        add_different_shape_property<Datatype>(
                            property_json, properties.collection<Properties>());
                    }
                    else if(datatype_json == "float64") {
                        using Datatype = double;

                        add_different_shape_property<Datatype>(
                            property_json, properties.collection<Properties>());
                    }
                    else if(datatype_json == "string") {
                        using Datatype = std::string;

                        add_different_shape_property<Datatype>(
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

            break;
        }
        case ValueVariability::variable: {

            std::string const shape_variability_json =
                property_json.at("shape_variability");
            auto const shape_variability =
                string_to_aspect<ShapeVariability>(shape_variability_json);

            switch(shape_per_object) {
                case ShapePerObject::same: {

                    switch(shape_variability) {
                        case ShapeVariability::constant: {
                            // variable_value / same_shape / constant_shape
                            using Properties = same_shape::constant_shape::Properties;

                            if(datatype_json == "uint32") {
                                using Datatype = std::uint32_t;

                                add_same_shape_constant_shape_property<Datatype>(
                                    property_json, properties.collection<Properties>());
                            }
                            else if(datatype_json == "uint64") {
                                using Datatype = std::uint64_t;

                                add_same_shape_constant_shape_property<Datatype>(
                                    property_json, properties.collection<Properties>());
                            }
                            else if(datatype_json == "float32") {
                                using Datatype = float;

                                add_same_shape_constant_shape_property<Datatype>(
                                    property_json, properties.collection<Properties>());
                            }
                            else if(datatype_json == "float64") {
                                using Datatype = double;

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
                        case ShapeVariability::variable: {
                            // variable_value / same_shape / variable_shape
                            // using Properties = same_shape::variable_shape::Properties;

                            throw_unsupported("same_shape::variable_shape");

                            break;
                        }
                    }

                    break;
                }
                case ShapePerObject::different: {

                    switch(shape_variability) {
                        case ShapeVariability::constant: {
                            // variable_value / different_shape / constant_shape
                            // using Properties = different_shape::constant_shape::Properties;

                            throw_unsupported("different_shape::constant_shape");

                            break;
                        }
                        case ShapeVariability::variable: {
                            // variable_value / different_shape / variable_shape
                            // using Properties = different_shape::variable_shape::Properties;

                            throw_unsupported("different_shape::variable_shape");

                            break;
                        }
                    }

                    break;
                }
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
        if(!contains(property_set_json, "space_domain")) {
            auto& property_set = property_sets.contains(name)
                ? property_sets[name]
                : property_sets.add(name)
                ;
            property_set_ref = property_set;
        }
        else {
            auto const& space_domain_json =
                property_set_json.at("space_domain");

            if(contains(space_domain_json, "space_box")) {
                SpaceConfiguration const space_configuration{
                        Mobility::stationary,
                        SpaceDomainItemType::box
                    };
                std::string const datatype_json =
                    space_domain_json.at("datatype");
                assert(datatype_json == "float64");
                auto const datatype = hdf5::native_float64;
                std::size_t rank = space_domain_json.at("rank");

                if(property_sets.contains(name)) {
                    // Append to the existing property set

                    // See if existing domain is the same as the one we
                    // just read
                    throw_unsupported(
                        "Check space domain is same as existing");
                }
                else {
                    // Create a new property set
                    auto& property_set =
                        property_sets.add(
                            name, space_configuration, datatype, rank);
                    property_set_ref = property_set;
                }

                PropertySet& property_set = *property_set_ref;

                add_space_boxes<StationarySpaceBox, double>(
                    space_domain_json.at("space_box"),
                    property_set.space_domain());
            }
            else {
                throw std::runtime_error(
                    "Could not find space domain values");
            }





            // add_object_tracker(
            //     property_set_json.at("object_tracker"),
            //     property_set.object_tracker());
        }
    }
    else {
        auto const& time_domain_json = property_set_json.at("time_domain");
        auto const& clock_json = time_domain_json.at("clock");
        auto const& epoch_json = clock_json.at("epoch");

        // TODO origin and calendar are optional
        time::Epoch::Kind const epoch_kind =
            string_to_aspect<time::Epoch::Kind>(epoch_json.at("kind"));
        std::string const epoch_origin = epoch_json.at("origin");
        time::Calendar const epoch_calendar =
            string_to_aspect<time::Calendar>(epoch_json.at("calendar"));
        time::Epoch const epoch{epoch_kind, epoch_origin, epoch_calendar};

        time::DurationCount const tick_period_count =
            clock_json.at("tick_period_count");
        std::string const unit = clock_json.at("unit");

        // TODO Read the time domain item type from the json
        TimeConfiguration const time_configuration{
                TimeDomainItemType::point
            };
        Clock const clock{
                epoch,
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

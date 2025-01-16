#include "lue/translate/format/json.hpp"
#include "lue/navigate.hpp"
#include "lue/translate/format/gdal.hpp"
#include "lue/utility/environment.hpp"
#include <optional>


namespace lue {
    namespace utility {
        namespace {

            void throw_unsupported(std::string const& message)
            {
                throw std::runtime_error(fmt::format("Unsupported feature used: {}", message));
            }


            void throw_missing_entry(
                std::string const& pathname, std::string const& location, std::string const& entry_name)
            {
                throw std::runtime_error(
                    fmt::format("Expected JSON entry '{}' at {} in {}", entry_name, location, pathname));
            }


            bool contains(::json const& json, std::string const& name)
            {
                return json.find(name) != json.end();
            }


            template<typename Collection>
            hdf5::Shape read_gdal_raster(std::string const& pathname, Collection& collection)
            {
                // Open dataset
                gdal::Raster raster{gdal::open_dataset(pathname, GDALAccess::GA_ReadOnly)};

                // Verify dataset is OK for us
                assert(raster.nr_bands() == 1);
                auto band = raster.band(1);

                assert(
                    gdal_data_type_to_memory_data_type(band.data_type()) ==
                    hdf5::native_datatype<typename Collection::value_type>());

                // Read all cells
                auto const [nr_rows, nr_cols] = raster.shape();
                collection.resize(nr_rows * nr_cols);
                band.read(collection.data());

                return {
                    static_cast<hdf5::Shape::value_type>(nr_rows),
                    static_cast<hdf5::Shape::value_type>(nr_cols)};
            }


            void add_object_tracker(
                ::json const& object_tracker_json, data_model::ObjectTracker& object_tracker)
            {
                if (contains(object_tracker_json, "active_set_index"))
                {

                    std::vector<data_model::Index> idxs = object_tracker_json.at("active_set_index");
                    auto& active_set_idx = object_tracker.active_set_index();

                    // When appending active sets, we have to update the indices
                    // read from the JSON. This also works when no active sets are
                    // stored already.
                    data_model::Index offset = object_tracker.active_object_id().nr_ids();
                    std::transform(
                        idxs.begin(),
                        idxs.end(),
                        idxs.begin(),
                        [offset](auto const idx) { return idx + offset; });

                    data_model::Index const begin = active_set_idx.nr_indices();
                    data_model::IndexRange const index_range{begin, begin + idxs.size()};
                    active_set_idx.expand(idxs.size());
                    active_set_idx.write(index_range, idxs.data());
                }

                if (contains(object_tracker_json, "active_object_index"))
                {

                    // Read indices from file
                    std::vector<data_model::Index> const idxs = object_tracker_json.at("active_object_index");
                    auto& active_object_idx = object_tracker.active_object_index();

                    // In order to append, we need to know for each object in the
                    // active set how many values are already stored. This information
                    // is not available yet here... FIXME

                    // Add indices to dataset
                    data_model::Index const begin = 0;
                    data_model::IndexRange const index_range{begin, begin + idxs.size()};
                    active_object_idx.expand(idxs.size());
                    active_object_idx.write(index_range, idxs.data());
                }

                if (contains(object_tracker_json, "active_object_id"))
                {

                    std::vector<data_model::ID> const ids = object_tracker_json.at("active_object_id");
                    auto& active_object_id = object_tracker.active_object_id();

                    data_model::Index const begin = active_object_id.nr_ids();
                    data_model::IndexRange const index_range{begin, begin + ids.size()};
                    active_object_id.expand(ids.size());
                    active_object_id.write(index_range, ids.data());
                }
            }


            void add_time_points(::json const& time_point_json, data_model::TimeDomain& time_domain)
            {
                std::vector<data_model::time::DurationCount> const time_points = time_point_json;
                auto value = time_domain.value<data_model::TimePoint>();

                data_model::Index const begin = value.nr_points();
                data_model::IndexRange const index_range{begin, begin + time_points.size()};
                value.expand(time_points.size());
                value.write(index_range, time_points.data());
            }


            template<typename Coordinate>
            std::vector<Coordinate> read_space_box(std::string const& dataset_name)
            {
                auto raster = gdal::Raster{gdal::open_dataset(dataset_name, GDALAccess::GA_ReadOnly)};
                auto const [nr_rows, nr_cols] = raster.shape();
                auto const [west, cell_width, row_rotation, north, col_rotation, cell_height] =
                    raster.geo_transform();
                double const east = west + nr_cols * cell_width;
                double const south = north - nr_rows * cell_height;

                std::array<double, 4> space_box{west, south, east, north};

                return std::vector<Coordinate>(space_box.begin(), space_box.end());
            }


            template<typename SpaceBox, typename Coordinate>
            void add_space_boxes(::json const& space_box_json, data_model::SpaceDomain& space_domain)
            {
                if (!space_box_json.empty())
                {

                    auto value = space_domain.value<data_model::StationarySpaceBox>();
                    auto const shape = value.array_shape();
                    auto const nr_elements_in_object_array = size_of_shape(shape, 1);
                    std::vector<Coordinate> space_boxes;

                    if (space_box_json.front().is_string())
                    {
                        // Read space box from datasets
                        std::vector<std::string> const dataset_names = space_box_json;

                        space_boxes.resize(dataset_names.size() * nr_elements_in_object_array);
                        auto it = space_boxes.begin();

                        for (std::string dataset_name : dataset_names)
                        {
                            dataset_name = expand_environment_variables(dataset_name);
                            auto space_box = read_space_box<Coordinate>(dataset_name);
                            it = std::copy(space_box.begin(), space_box.end(), it);
                        }
                    }
                    else
                    {
                        // Read space box from JSON
                        space_boxes = space_box_json.get<decltype(space_boxes)>();

                        if (space_boxes.size() % nr_elements_in_object_array != 0)
                        {
                            throw std::runtime_error(fmt::format(
                                "Expected a multiple of {} coordinates, but found {} ({})",
                                nr_elements_in_object_array,
                                space_boxes.size(),
                                space_domain.id().pathname()));
                        }
                    }

                    auto const nr_object_arrays = space_boxes.size() / nr_elements_in_object_array;

                    data_model::Index const begin = value.nr_boxes();
                    data_model::IndexRange const index_range{begin, begin + nr_object_arrays};
                    value.expand(nr_object_arrays);
                    value.write(index_range, space_boxes.data());
                }
            }


            void add_space_domain_items(
                ::json const& item_type_json,
                hdf5::Datatype const& datatype,
                data_model::PropertySet& property_set)
            {
                data_model::SpaceConfiguration const& space_configuration{
                    property_set.space_domain().configuration()};

                switch (space_configuration.value<data_model::Mobility>())
                {
                    case data_model::Mobility::stationary:
                    {

                        switch (space_configuration.value<data_model::SpaceDomainItemType>())
                        {

                            case data_model::SpaceDomainItemType::point:
                            {
                                throw std::runtime_error(fmt::format(
                                    "Importing stationary space points not supported yet ({})",
                                    property_set.space_domain().id().pathname()));
                                // break;
                            }

                            case data_model::SpaceDomainItemType::box:
                            {
                                if (datatype == hdf5::native_float64)
                                {
                                    add_space_boxes<data_model::StationarySpaceBox, double>(
                                        item_type_json, property_set.space_domain());
                                }
                                else
                                {
                                    throw_unsupported("space domain coordinate type");
                                }

                                break;
                            }
                        }

                        break;
                    }
                    case data_model::Mobility::mobile:
                    {
                        throw std::runtime_error(fmt::format(
                            "Importing mobile space items not supported yet ({})",
                            property_set.id().pathname()));

                        break;
                    }
                }
            }


            template<typename Datatype>
            void add_same_shape_property(
                ::json const& property_json, data_model::same_shape::Properties& properties)
            {
                std::string const name = property_json.at("name");

                hdf5::Shape shape;

                if (contains(property_json, "shape"))
                {
                    shape = property_json.at("shape");
                }

                auto const nr_elements_in_object_array = size_of_shape(shape, 1);
                auto const datatype = hdf5::native_datatype<Datatype>();
                std::vector<Datatype> const values = property_json.at("value");

                if (values.size() % nr_elements_in_object_array != 0)
                {
                    throw std::runtime_error(fmt::format(
                        "Number of values is not a multiple of the number of elements "
                        "in an object array ({} % {} != 0)",
                        values.size(),
                        nr_elements_in_object_array));
                }

                auto& property =
                    properties.contains(name) ? properties[name] : properties.add(name, datatype, shape);

                data_model::Index const begin = property.value().nr_arrays();
                auto const nr_object_arrays = values.size() / nr_elements_in_object_array;
                data_model::IndexRange const index_range{begin, begin + nr_object_arrays};
                property.value().expand(nr_object_arrays);
                property.value().write(index_range, values.data());


                // TODO(KDJ)
                // datatype, shape must match, etc
                // branch on contains()
            }


            template<typename Datatype>
            void add_different_shape_property(
                ::json const& property_json, data_model::different_shape::Properties& properties)
            {
                std::string const name = property_json.at("name");
                std::size_t rank = property_json.at("rank");
                auto const datatype = hdf5::native_datatype<Datatype>();
                auto const value_json = property_json.at("value");

                auto& property =
                    properties.contains(name) ? properties[name] : properties.add(name, datatype, rank);

                hdf5::Shape shape;
                std::vector<Datatype> values;

                for (auto const& a_value_json : value_json)
                {
                    data_model::ID const id = a_value_json.at("id");

                    if (contains(a_value_json, "dataset"))
                    {
                        // Value is stored in an external dataset
                        std::string const dataset_name =
                            expand_environment_variables(a_value_json.at("dataset"));
                        shape = read_gdal_raster(dataset_name, values);
                    }
                    else
                    {
                        // Value is stored inline
                        // BOGUS?
                        shape = a_value_json.at("shape");
                        // values = a_value_json.get<decltype(values)>();
                        values = a_value_json.at("value").get<decltype(values)>();
                    }

                    // Each object has a uniquely shaped value. This value does not
                    // change over time, so it cannot be appended to. It must not
                    // already exist.
                    // assert(!property.value().exists(id));

                    property.value().expand(1, &id, &shape);
                    property.value()[id].write(values.data());
                }

                if (contains(property_json, "space_discretization"))
                {
                    auto const discretization_json = property_json.at("space_discretization");
                    auto const discretization_type =
                        data_model::string_to_aspect<data_model::SpaceDiscretization>(
                            discretization_json.at("type"));

                    std::string const property_path = discretization_json.at("property");

                    // Give the current property, navigate to the property pointed
                    // to by the path
                    auto discretization_property = lue::data_model::property(property, property_path);

                    property.set_space_discretization(discretization_type, discretization_property);
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
            //     // TODO(KDJ)
            //     // datatype, shape must match, etc
            //     // branch on contains()
            //
            // }


            template<typename Datatype>
            void add_same_shape_constant_shape_property(
                ::json const& property_json, data_model::same_shape::constant_shape::Properties& properties)
            {
                std::string const name = property_json.at("name");

                std::string const datatype_json = property_json.at("datatype");
                assert(datatype_json == "uint64");

                auto const datatype = hdf5::Datatype{hdf5::NativeDatatypeTraits<Datatype>::type_id()};

                hdf5::Shape shape;
                if (contains(property_json, "shape"))
                {
                    shape = property_json.at("shape");
                }
                auto const nr_elements_in_object_array = size_of_shape(shape, 1);

                auto& property =
                    properties.contains(name) ? properties[name] : properties.add(name, datatype, shape);

                // TODO(KDJ)
                // datatype, shape must match, etc
                // branch on contains()

                std::vector<Datatype> const values = property_json.at("value");
                // Index const begin = property.value().nr_arrays();
                // IndexRange const index_range{begin, begin + values.size()};
                // property.value().expand(values.size());
                // property.value().write(index_range, values.data());

                if (values.size() % nr_elements_in_object_array != 0)
                {
                    throw std::runtime_error(fmt::format(
                        "Number of values is not a multiple of the number of elements "
                        "in an object array ({} % {} != 0)",
                        values.size(),
                        nr_elements_in_object_array));
                }

                auto const nr_object_arrays = values.size() / nr_elements_in_object_array;
                data_model::Index const begin = property.value().nr_arrays();
                data_model::IndexRange const index_range{begin, begin + nr_object_arrays};
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
            //     // TODO(KDJ)
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


            template<typename Datatype>
            void add_different_shape_constant_shape_property(
                ::json const& property_json,
                data_model::different_shape::constant_shape::Properties& properties)
            {
                std::string const name = property_json.at("name");
                std::size_t const rank = property_json.at("rank");
                auto const datatype = hdf5::Datatype{hdf5::NativeDatatypeTraits<Datatype>::type_id()};

                auto& property =
                    properties.contains(name) ? properties[name] : properties.add(name, datatype, rank);

                auto& value = property.value();

                hdf5::Shape shape;
                std::vector<Datatype> values;
                auto const& value_json = property_json.at("value");

                for (auto const& a_value_json : value_json)
                {
                    data_model::ID const id = a_value_json.at("id");

                    if (contains(a_value_json, "dataset"))
                    {
                        // Value is stored in an external dataset
                        std::string const dataset_name =
                            expand_environment_variables(a_value_json.at("dataset"));
                        shape = read_gdal_raster(dataset_name, values);
                    }
                    // else {
                    //     // Value is stored inline
                    //     // BOGUS?
                    //     values = a_value_json.get<decltype(values)>();
                    //     shape = a_value_json.at("shape");
                    // }

                    assert(!value.exists(id));

                    data_model::Count nr_locations_in_time = 1;

                    value.expand(id, shape, nr_locations_in_time);
                    value[id].write(values.data());
                }


                // if(contains(property_json, "space_discretization")) {
                //     auto const discretization_json =
                //         property_json.at("space_discretization");
                //     auto const discretization_type =
                //         string_to_aspect<SpaceDiscretization>(
                //             discretization_json.at("type"));

                //     std::string const property_path = discretization_json.at("property");

                //     // Give the current property, navigate to the property pointed
                //     // to by the path
                //     auto discretization_property = lue::property(property, property_path);

                //     property.set_space_discretization(
                //         discretization_type, discretization_property);
                // }
            }


            template<typename Datatype>
            void add_different_shape_variable_shape_property(
                ::json const& property_json,
                data_model::different_shape::variable_shape::Properties& properties)
            {
                std::string const name = property_json.at("name");
                std::size_t const rank = property_json.at("rank");

                std::string const datatype_json = property_json.at("datatype");
                assert(datatype_json == "uint64");

                auto const datatype = hdf5::Datatype{hdf5::NativeDatatypeTraits<Datatype>::type_id()};

                auto& property =
                    properties.contains(name) ? properties[name] : properties.add(name, datatype, rank);

                // TODO(KDJ)
                // datatype, rank must match, etc
                // branch on contains()

                std::vector<std::vector<Datatype>> const values = property_json.at("value");

                auto& value = property.value();

                std::vector<hdf5::Shape> const shape = property_json.at("shape");
                std::vector<data_model::Index> const object_id = property_json.at("object_id");
                assert(values.size() == object_id.size());
                assert(shape.size() == object_id.size());
                auto const nr_objects = shape.size();

                auto a =
                    value.expand(value.nr_locations_in_time(), nr_objects, object_id.data(), shape.data());

                for (std::size_t o = 0; o < nr_objects; ++o)
                {
                    a[object_id[o]].write(values[o].data());
                }
            }


            void add_property(::json const& property_json, data_model::Properties& properties)
            {
                // std::string const description = property_json["description"];

                std::string const value_variability_json = property_json.at("value_variability");
                auto const value_variability =
                    data_model::string_to_aspect<data_model::ValueVariability>(value_variability_json);

                std::string const shape_per_object_json = property_json.at("shape_per_object");
                auto const shape_per_object =
                    data_model::string_to_aspect<data_model::ShapePerObject>(shape_per_object_json);

                std::string const datatype_json = property_json.at("datatype");

                switch (value_variability)
                {
                    case data_model::ValueVariability::constant:
                    {

                        switch (shape_per_object)
                        {
                            case data_model::ShapePerObject::same:
                            {
                                // constant_value / same_shape
                                using Properties = data_model::same_shape::Properties;

                                if (datatype_json == "uint32")
                                {
                                    using Datatype = std::uint32_t;

                                    add_same_shape_property<Datatype>(
                                        property_json, properties.collection<Properties>());
                                }
                                else if (datatype_json == "uint64")
                                {
                                    using Datatype = std::uint64_t;

                                    add_same_shape_property<Datatype>(
                                        property_json, properties.collection<Properties>());
                                }
                                else if (datatype_json == "float32")
                                {
                                    using Datatype = float;

                                    add_same_shape_property<Datatype>(
                                        property_json, properties.collection<Properties>());
                                }
                                else if (datatype_json == "float64")
                                {
                                    using Datatype = double;

                                    add_same_shape_property<Datatype>(
                                        property_json, properties.collection<Properties>());
                                }
                                else if (datatype_json == "string")
                                {
                                    using Datatype = std::string;

                                    add_same_shape_property<Datatype>(
                                        property_json, properties.collection<Properties>());
                                }
                                else
                                {
                                    throw std::runtime_error(
                                        fmt::format("Datatype {} not supported yet", datatype_json));
                                }

                                break;
                            }
                            case data_model::ShapePerObject::different:
                            {
                                // constant_value / different_shape
                                using Properties = data_model::different_shape::Properties;

                                if (datatype_json == "uint32")
                                {
                                    using Datatype = std::uint32_t;

                                    add_different_shape_property<Datatype>(
                                        property_json, properties.collection<Properties>());
                                }
                                else if (datatype_json == "uint64")
                                {
                                    using Datatype = std::uint64_t;

                                    add_different_shape_property<Datatype>(
                                        property_json, properties.collection<Properties>());
                                }
                                else if (datatype_json == "float32")
                                {
                                    using Datatype = float;

                                    add_different_shape_property<Datatype>(
                                        property_json, properties.collection<Properties>());
                                }
                                else if (datatype_json == "float64")
                                {
                                    using Datatype = double;

                                    add_different_shape_property<Datatype>(
                                        property_json, properties.collection<Properties>());
                                }
                                else if (datatype_json == "string")
                                {
                                    using Datatype = std::string;

                                    add_different_shape_property<Datatype>(
                                        property_json, properties.collection<Properties>());
                                }
                                else
                                {
                                    throw std::runtime_error(
                                        fmt::format("Datatype {} not supported yet", datatype_json));
                                }

                                break;
                            }
                        }

                        break;
                    }
                    case data_model::ValueVariability::variable:
                    {

                        std::string const shape_variability_json = property_json.at("shape_variability");
                        auto const shape_variability =
                            data_model::string_to_aspect<data_model::ShapeVariability>(
                                shape_variability_json);

                        switch (shape_per_object)
                        {
                            case data_model::ShapePerObject::same:
                            {

                                switch (shape_variability)
                                {
                                    case data_model::ShapeVariability::constant:
                                    {
                                        // variable_value / same_shape / constant_shape
                                        using Properties = data_model::same_shape::constant_shape::Properties;

                                        if (datatype_json == "uint32")
                                        {
                                            using Datatype = std::uint32_t;

                                            add_same_shape_constant_shape_property<Datatype>(
                                                property_json, properties.collection<Properties>());
                                        }
                                        else if (datatype_json == "uint64")
                                        {
                                            using Datatype = std::uint64_t;

                                            add_same_shape_constant_shape_property<Datatype>(
                                                property_json, properties.collection<Properties>());
                                        }
                                        else if (datatype_json == "float32")
                                        {
                                            using Datatype = float;

                                            add_same_shape_constant_shape_property<Datatype>(
                                                property_json, properties.collection<Properties>());
                                        }
                                        else if (datatype_json == "float64")
                                        {
                                            using Datatype = double;

                                            add_same_shape_constant_shape_property<Datatype>(
                                                property_json, properties.collection<Properties>());
                                        }
                                        else
                                        {
                                            throw std::runtime_error(
                                                fmt::format("Datatype {} not supported yet", datatype_json));
                                        }

                                        break;
                                    }
                                    case data_model::ShapeVariability::variable:
                                    {
                                        // variable_value / same_shape / variable_shape
                                        // using Properties = same_shape::variable_shape::Properties;

                                        throw_unsupported("same_shape::variable_shape");

                                        break;
                                    }
                                }

                                break;
                            }
                            case data_model::ShapePerObject::different:
                            {

                                switch (shape_variability)
                                {
                                    case data_model::ShapeVariability::constant:
                                    {
                                        // variable_value / different_shape / constant_shape
                                        using Properties =
                                            data_model::different_shape::constant_shape::Properties;

                                        if (datatype_json == "uint8")
                                        {
                                            using Datatype = std::uint8_t;

                                            add_different_shape_constant_shape_property<Datatype>(
                                                property_json, properties.collection<Properties>());
                                        }

                                        // TODO(KDJ) More value types

                                        break;
                                    }
                                    case data_model::ShapeVariability::variable:
                                    {
                                        // variable_value / different_shape / variable_shape
                                        using Properties =
                                            data_model::different_shape::variable_shape::Properties;

                                        if (datatype_json == "uint32")
                                        {
                                            using Datatype = std::uint32_t;

                                            add_different_shape_variable_shape_property<Datatype>(
                                                property_json, properties.collection<Properties>());
                                        }
                                        else if (datatype_json == "uint64")
                                        {
                                            using Datatype = std::uint64_t;

                                            add_different_shape_variable_shape_property<Datatype>(
                                                property_json, properties.collection<Properties>());
                                        }
                                        else if (datatype_json == "float32")
                                        {
                                            using Datatype = float;

                                            add_different_shape_variable_shape_property<Datatype>(
                                                property_json, properties.collection<Properties>());
                                        }
                                        else if (datatype_json == "float64")
                                        {
                                            using Datatype = double;

                                            add_different_shape_variable_shape_property<Datatype>(
                                                property_json, properties.collection<Properties>());
                                        }
                                        else
                                        {
                                            throw std::runtime_error(
                                                fmt::format("Datatype {} not supported yet", datatype_json));
                                        }

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


            std::tuple<data_model::SpaceConfiguration, hdf5::Datatype, std::size_t, ::json>
            parse_space_domain(::json const& space_domain_json)
            {
                data_model::Mobility mobility{data_model::Mobility::stationary};
                data_model::SpaceDomainItemType item_type{};
                ::json item_type_json;

                if (contains(space_domain_json, "space_box"))
                {
                    item_type_json = space_domain_json.at("space_box");
                    item_type = data_model::SpaceDomainItemType::box;
                }
                else
                {
                    throw std::runtime_error("Could not find space domain values");
                }

                std::string const datatype_json = space_domain_json.at("datatype");
                assert(datatype_json == "float64");
                auto const datatype = hdf5::native_float64;
                std::size_t const rank = space_domain_json.at("rank");

                return std::make_tuple(
                    data_model::SpaceConfiguration{mobility, item_type}, datatype, rank, item_type_json);
            }


            void verify_time_domain_is_compatible(
                data_model::PropertySet const& property_set,
                data_model::TimeConfiguration const& time_configuration,
                data_model::Clock const& clock)
            {
                if (property_set.time_domain().configuration() != time_configuration)
                {
                    throw std::runtime_error(fmt::format(
                        "Existing time configuration at {} does not match "
                        "the one in the JSON file",
                        property_set.id().pathname()));
                }

                if (property_set.time_domain().clock() != clock)
                {
                    throw std::runtime_error(fmt::format(
                        "Existing clock at {} does not match "
                        "the one in the JSON file",
                        property_set.id().pathname()));
                }
            }


            void add_property_set(::json const& property_set_json, data_model::PropertySets& property_sets)
            {
                std::string const name = property_set_json.at("name");

                using PropertySetRef = std::reference_wrapper<data_model::PropertySet>;
                using OptionalPropertySet = std::optional<PropertySetRef>;

                OptionalPropertySet property_set_ref;

                if (!contains(property_set_json, "time_domain"))
                {
                    if (!contains(property_set_json, "space_domain"))
                    {

                        // Add/use property-set without time domain and with space domain
                        auto& property_set =
                            property_sets.contains(name) ? property_sets[name] : property_sets.add(name);
                        property_set_ref = property_set;
                    }
                    else
                    {

                        auto const& [space_configuration, datatype, rank, space_domain_item_type_json] =
                            parse_space_domain(property_set_json.at("space_domain"));

                        // Add/use property-set without time domain and with space domain
                        if (property_sets.contains(name))
                        {
                            auto& property_set = property_sets[name];

                            // FIXME
                            // verify_space_domain_is_compatible(
                            //     property_set, space_configuration)

                            throw_unsupported("Check space domain is same as existing");

                            property_set_ref = property_set;
                        }
                        else
                        {
                            // Create a new property set
                            auto& property_set = property_sets.add(name, space_configuration, datatype, rank);
                            property_set_ref = property_set;
                        }

                        data_model::PropertySet& property_set = *property_set_ref;

                        // No time domain, so object tracker is not needed...
                        // add_object_tracker(
                        //     property_set_json.at("object_tracker"),
                        //     property_set.object_tracker());

                        add_space_domain_items(space_domain_item_type_json, datatype, property_set);
                    }
                }
                else
                {
                    auto const& time_domain_json = property_set_json.at("time_domain");
                    auto const& clock_json = time_domain_json.at("clock");
                    auto const& epoch_json = clock_json.at("epoch");

                    auto const epoch_kind =
                        data_model::string_to_aspect<data_model::time::Epoch::Kind>(epoch_json.at("kind"));
                    data_model::time::Epoch epoch{epoch_kind};

                    if (epoch_json.contains("origin"))
                    {
                        std::string const epoch_origin = epoch_json.at("origin");

                        if (epoch_json.contains("calendar"))
                        {
                            auto const epoch_calendar =
                                data_model::string_to_aspect<data_model::time::Calendar>(
                                    epoch_json.at("calendar"));
                            epoch = data_model::time::Epoch{epoch_kind, epoch_origin, epoch_calendar};
                        }
                        else
                        {
                            epoch = data_model::time::Epoch{epoch_kind, epoch_origin};
                        }
                    }

                    data_model::time::DurationCount const tick_period_count =
                        clock_json.at("tick_period_count");
                    std::string const unit = clock_json.at("unit");

                    // FIXME Read the time domain item type from the json
                    data_model::TimeConfiguration const time_configuration{
                        data_model::TimeDomainItemType::point};
                    data_model::Clock const clock{
                        epoch, data_model::string_to_aspect<data_model::time::Unit>(unit), tick_period_count};

                    if (!contains(property_set_json, "space_domain"))
                    {

                        // Add/use property-set with time domain and without space domain
                        if (property_sets.contains(name))
                        {
                            auto& property_set = property_sets[name];

                            verify_time_domain_is_compatible(property_set, time_configuration, clock);

                            property_set_ref = property_set;
                        }
                        else
                        {
                            auto& property_set = property_sets.add(name, time_configuration, clock);
                            property_set_ref = property_set;
                        }
                    }
                    else
                    {

                        auto const& [space_configuration, datatype, rank, space_domain_item_type_json] =
                            parse_space_domain(property_set_json.at("space_domain"));

                        // Add/use property-set with time domain and with space domain
                        if (property_sets.contains(name))
                        {
                            auto& property_set = property_sets[name];

                            verify_time_domain_is_compatible(property_set, time_configuration, clock);
                            // FIXME
                            // verify_space_domain_is_compatible(
                            //     property_set, space_configuration)

                            throw_unsupported("Check space domain is same as existing");

                            property_set_ref = property_set;
                        }
                        else
                        {
                            // Create a new property set
                            auto& property_set = property_sets.add(
                                name, time_configuration, clock, space_configuration, datatype, rank);
                            property_set_ref = property_set;
                        }

                        data_model::PropertySet& property_set = *property_set_ref;

                        add_space_domain_items(space_domain_item_type_json, datatype, property_set);
                    }

                    assert(property_set_ref);
                    data_model::PropertySet& property_set = *property_set_ref;

                    add_object_tracker(property_set_json.at("object_tracker"), property_set.object_tracker());

                    add_time_points(time_domain_json.at("time_point"), property_set.time_domain());
                }

                assert(property_set_ref);

                if (contains(property_set_json, "properties"))
                {
                    data_model::PropertySet& property_set_ = *property_set_ref;

                    for (auto const& property_json : property_set_json.at("properties"))
                    {
                        add_property(property_json, property_set_.properties());
                    }
                }
            }


            void add_phenomenon(::json const& phenomenon_json, data_model::Phenomena& phenomena)
            {
                std::string const name = phenomenon_json.at("name");
                auto& phenomenon = phenomena.contains(name) ? phenomena[name] : phenomena.add(name);

                if (contains(phenomenon_json, "object_id"))
                {
                    // JSON contains IDs of objects having information that doesn't
                    // change through time.
                    std::vector<data_model::Index> const object_id = phenomenon_json.at("object_id");

                    // This logic should work when this is the first collection of IDs
                    // and when this collection should be appended to an existing
                    // collection of IDs
                    data_model::Index const begin = phenomenon.object_id().nr_arrays();
                    data_model::IndexRange const index_range{begin, begin + object_id.size()};
                    phenomenon.object_id().expand(object_id.size());
                    phenomenon.object_id().write(index_range, object_id.data());
                }

                if (contains(phenomenon_json, "collection_property_sets"))
                {
                    for (auto const& property_set_json : phenomenon_json.at("collection_property_sets"))
                    {
                        add_property_set(property_set_json, phenomenon.collection_property_sets());
                    }
                }

                if (contains(phenomenon_json, "property_sets"))
                {
                    for (auto const& property_set_json : phenomenon_json.at("property_sets"))
                    {
                        add_property_set(property_set_json, phenomenon.property_sets());
                    }
                }
            }


            void add_universe(::json const& universe_json, data_model::Universes& universes)
            {
                std::string const name = universe_json.at("name");

                // cppcheck-suppress variableScope
                auto& universe = universes.contains(name) ? universes[name] : universes.add(name);

                if (contains(universe_json, "phenomena"))
                {
                    for (auto const& phenomenon_json : universe_json.at("phenomena"))
                    {
                        add_phenomenon(phenomenon_json, universe.phenomena());
                    }
                }
            }


            void translate_json_to_lue(::json const& lue_json, data_model::Dataset& dataset)
            {
                if (!contains(lue_json, "dataset"))
                {
                    throw_missing_entry("<json>", "root", "dataset");
                }

                auto const& dataset_json = lue_json.at("dataset");

                if (contains(dataset_json, "universes"))
                {
                    for (auto const& universe_json : dataset_json.at("universes"))
                    {
                        add_universe(universe_json, dataset.universes());
                    }
                }

                if (contains(dataset_json, "phenomena"))
                {
                    for (auto const& phenomenon_json : dataset_json.at("phenomena"))
                    {
                        add_phenomenon(phenomenon_json, dataset.phenomena());
                    }
                }
            }

        }  // Anonymous namespace


        data_model::Dataset translate_json_to_lue(::json const& lue_json, std::string const& dataset_name)
        {
            auto dataset = data_model::create_in_memory_dataset(dataset_name);

            translate_json_to_lue(lue_json, dataset);

            return dataset;
        }


        void translate_json_to_lue(::json const& lue_json, std::string const& lue_pathname, bool const add)
        {
            // Either create a new dataset or add the information stored in the
            // JSON to an existing dataset
            auto dataset = add && data_model::dataset_exists(lue_pathname)
                               ? data_model::Dataset{lue_pathname, H5F_ACC_RDWR}
                               : data_model::create_dataset(lue_pathname);

            translate_json_to_lue(lue_json, dataset);
        }


        void translate_json_to_lue(
            std::string const& json_pathname,
            std::string const& lue_pathname,
            bool const add,
            Metadata const& /* metadata */)
        {
            std::ifstream stream(json_pathname);

            if (!stream)
            {
                throw std::runtime_error("Cannot open " + json_pathname);
            }

            ::json lue_json;
            stream >> lue_json;

            if (!contains(lue_json, "dataset"))
            {
                throw_missing_entry(json_pathname, "root", "dataset");
            }

            return translate_json_to_lue(lue_json, lue_pathname, add);
        }

    }  // namespace utility
}  // namespace lue

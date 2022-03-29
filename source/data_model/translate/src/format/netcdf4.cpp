#include "lue/translate/format/netcdf4.hpp"
#include "lue/data_model/netcdf4.hpp"


namespace lue::utility {

    namespace ldm = lue::data_model;
    namespace lh5 = lue::hdf5;
    namespace ln4 = ldm::netcdf4;


    ldm::Dataset open_dataset(
        std::string const& dataset_name,
        bool append_data)
    {
        // Create / open dataset
        auto create_dataset =
            [dataset_name]()
            {
                return ldm::create_dataset(dataset_name);
            };

        auto open_dataset =
            [dataset_name, append_data]()
            {
                if(!append_data)
                {
                    throw std::runtime_error(fmt::format("Dataset {} already exists", dataset_name));
                }

                return ldm::open_dataset(dataset_name);
            };

        return !ldm::dataset_exists(dataset_name) ? create_dataset() : open_dataset();
    }


    lh5::Datatype netcdf_data_type_to_hdf5_data_type(
        nc_type const data_type)
    {
        ::hid_t hdf5_type_id;

        switch(data_type)
        {
            case NC_FLOAT:
            {
                hdf5_type_id = lue::hdf5::NativeDatatypeTraits<float>::type_id();
                break;
            }
            case NC_DOUBLE:
            {
                hdf5_type_id = lue::hdf5::NativeDatatypeTraits<double>::type_id();
                break;
            }
            default:
            {
                throw std::runtime_error("Unhandled netcdf data type");
                break;
            }

        // NC_BYTE   -> int8_t
        // NC_SHORT  -> int16_t
        // NC_INT    -> int32_t
        // NC_INT64  -> int64_t
        //
        // NC_UBYTE  -> uint8_t
        // NC_USHORT -> uint16_t
        // NC_UINT   -> uint32_t
        // NC_UINT64 -> uint64_t
        //
        // NC_STRING -> string
        // NC_CHAR   -> ISO/ASCII character
        }

        return lh5::Datatype{hdf5_type_id};
    }


    template<
        typename T>
    void write_space_box(
        ldm::StationarySpaceBox& value)
    {
        std::vector<T> box(4);  // nr_areas * rank * 2
        box[0] = 0;  // west
        box[1] = -180;  // south
        box[2] = 360;  // east
        box[3] = 180;  // north

        value.write(box.data());
    }


    void write_space_box(
        lh5::Datatype space_coordinate_data_type,
        ldm::StationarySpaceBox& value)
    {
        if(space_coordinate_data_type == lh5::native_float32)
        {
            write_space_box<float>(value);
        }
        else if(space_coordinate_data_type == lh5::native_float64)
        {
            write_space_box<double>(value);
        }
    }


    ln4::Dimension const& dimension_by_name(
        ln4::Dimensions const& dimensions,
        std::string const& name)
    {
        return *std::find_if(dimensions.begin(), dimensions.end(),
                [name](ln4::Dimension const& dimension)
                {
                    return dimension.name() == name;
                }
            );
    }


    /*!
        @brief      Import NetCDF4 data into a LUE formatted file
        @param      input_dataset_name Name of NetCDF4 file
        @param      output_dataset_name Name of LUE file
        @param      append_dataset Whether or not to append data to an existing dataset, or to
                    create a new dataset
        @param      metadata
        @exception  .
    */
    void translate_netcdf4_dataset_to_lue(
        std::string const& input_dataset_name,
        std::string const& output_dataset_name,
        bool const append_data,
        Metadata const& metadata)
    {
        // Open input dataset, for reading
        ln4::File input_dataset{ln4::open_file(input_dataset_name)};

        // We assume the input dataset contains information that is organized according to the
        // CF convention. If this is not the case, try reading everything anyway. Bail out if
        // we encounter stuff we can't interpret (yet).

        // Sub-groups are not supported yet, so bail out if there are any
        if(!input_dataset.groups().empty())
        {
            throw std::runtime_error("Importing subgroups is not supported yet");
        }

        // Variables end up as properties in the LUE dataset. Which one depends on the
        // dimensions. Variables that have the same dimensions can be grouped in the same
        // property-set.

        std::string const input_dataset_stem{std::filesystem::path(input_dataset_name).stem().string()};
        std::string phenomenon_name = input_dataset_stem;

        /// auto const& root_json = metadata.object();

        /// if(json::has_key(root_json, "datasets"))
        /// {
        ///     auto const datasets_json = json::object(root_json, JSONPointer{"/datasets"});
        ///     auto const dataset_json_it = json::find(datasets_json, "name", input_dataset_stem);

        ///     if(dataset_json_it != datasets_json.end())
        ///     {
        ///         auto const dataset_json = *dataset_json_it;

        ///         if(json::has_key(dataset_json, "phenomenon"))
        ///         {
        ///             auto const phenomenon_pointer = json::pointer(dataset_json, "phenomenon");
        ///             auto const phenomenon_json = json::object(root_json, phenomenon_pointer);
        ///             phenomenon_name = json::string(phenomenon_json, "name");

        ///             // if(json::has_key(phenomenon_json, "property_set"))
        ///             // {
        ///             //     property_set_name = json::string(phenomenon_json,
        ///             //             JSON::json_pointer("/property_set/name"));
        ///             // }
        ///         }
        ///     }

        ///     // if(json::has_key(datasets_json, "name")

        ///     // auto const dataset_json = json::object(
        ///     //     root_json, JSONPointer{"/datasets"}, "name", input_dataset_stem);
        /// }

        // Open output dataset, for writing
        ldm::Dataset output_dataset{open_dataset(output_dataset_name, append_data)};
        ldm::Phenomena& phenomena{output_dataset.phenomena()};

        if(phenomena.contains(phenomenon_name))
        {
            throw std::runtime_error(fmt::format("Phenomenon {} already exists", phenomenon_name));
        }

        ldm::Phenomenon& phenomenon{phenomena.add(phenomenon_name)};

        // Add an ID for the one and only object
        {
            phenomenon.object_id().expand(1);
            ldm::ID object_id{0};  // Whatever
            phenomenon.object_id().write(&object_id);
        }

        ldm::PropertySets& property_sets{phenomenon.property_sets()};

        // Group variables by dimension IDs. Each of these sets of variables ends up in the
        // same property-set.
        {
            auto const variables_by_dimensions{ln4::group_variables(input_dataset)};

            for(auto const& [dimensions, variables]: variables_by_dimensions)
            {
                if(ln4::is_geographic_coordinate_system(dimensions))
                {
                    // Create property-set, including the properties for the space dimensions
                    std::string const property_set_name = "earth";
                    ldm::SpaceConfiguration const space_configuration{
                        ldm::Mobility::stationary,
                        ldm::SpaceDomainItemType::box};

                    assert(
                        input_dataset.variable("lat").data_type() ==
                        input_dataset.variable("lon").data_type());
                    nc_type const netcdf_data_type{input_dataset.variable("lat").data_type()};
                    lh5::Datatype const space_coordinate_data_type{
                        netcdf_data_type_to_hdf5_data_type(netcdf_data_type)};
                    std::size_t const rank{2};

                    ldm::PropertySet property_set{property_sets.add(property_set_name,
                        space_configuration, space_coordinate_data_type, rank)};

                    // Space domain
                    {
                        auto& space_domain = property_set.space_domain();
                        auto value = space_domain.value<ldm::StationarySpaceBox>();
                        value.expand(1);

                        write_space_box(space_coordinate_data_type, value);
                    }

                    // Discretization property
                    std::string const discretization_property_name = "grid";
                    {
                        using Extent = lh5::Shape::value_type;
                        lh5::Datatype const shape_datatype{lh5::NativeDatatypeTraits<Extent>::type_id()};
                        auto& discretization_property = property_set.properties().add(
                            discretization_property_name, shape_datatype,
                            lue::hdf5::Shape{2});  // nr rows, nr_cols
                        {
                            ln4::Dimension const& lat_dimension{dimension_by_name(dimensions, "lat")};
                            ln4::Dimension const& lon_dimension{dimension_by_name(dimensions, "lon")};

                            Extent nr_rows = lat_dimension.length();
                            Extent nr_cols = lon_dimension.length();
                            std::cout << nr_rows << " " << nr_cols << std::endl;
                            std::vector<lue::hdf5::Shape::value_type> shapes(2);  // nr_areas * rank
                            shapes[0] = nr_rows;
                            shapes[1] = nr_cols;

                            auto& value = discretization_property.value();
                            value.expand(1);
                            value.write(shapes.data());
                        }
                    }

                    // Properties
                    {
                        for(auto const& variable: variables)
                        {
                            // TODO, for all properties
                            // auto& elevation_property = property_set.properties().add(
                            //     elevation_property_name, elevation_datatype, rank);
                            // {
                            //     auto& value = elevation_property.value();
                            //     value.expand(nr_areas, ids.data(), shapes.data());

                            //     for(std::size_t o = 0; o < nr_areas; ++o) {
                            //         value[ids[o]].write(values[o].data());
                            //     }
                            // }

                            // // Link from elevation property to discretization property
                            // elevation_property.set_space_discretization(
                            //     ldm::SpaceDiscretization::regular_grid,
                            //     discretization_property);
                        }
                    }


                    // // Create properties
                    // for(auto const& variable: variables)
                    // {
                    //     assert(!variable.is_coordinate_variable());

                    //     std::cout << variable.name() << "\n";

                    //     // TODO

                    //     // Create property. It must not already exist.

                    //     // Write property-values.
                    // }
                }
            }
        }

        // TODO Now do the export part first







        // For each property-set to create:
        // - Add property-set
        //     - Add time domain
        //     - Add space domain
        // - Add properties
        // - Handle attributes



        // TODO
        // - Retrieve a list of attribute names.
        // - Pick the interesting ones, that tweak our behaviour.
        // - Copy the other ones. These should end up in the export as well.



        // /// // std::string conventions = input_dataset.attribute("Conventions");
        // std::string title = input_dataset.attribute("title");

        // std::cout << "title: >" << title << "<" << std::endl;
    }


    /*!
        @brief      Export LUE data into a NetCDF4 formatted file
        @param      input_dataset Input dataset
        @param      netcdf4_filename Name of NetCDF4 file
        @param      metadata
        @exception  .
    */
    void translate_lue_dataset_to_netcdf4(
        data_model::Dataset& input_dataset,
        std::string const& netcdf4_filename,
        [[maybe_unused]] Metadata const& metadata)
    {
        // Open output dataset, for reading
        ln4::File output_dataset{ln4::create_file(netcdf4_filename)};


        // TODO
    }

}  // namespace lue::utility

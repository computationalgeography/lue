#include "lue/gdal/compare_rasters.hpp"
#include "lue/gdal/data_type.hpp"
#include "lue/gdal/raster.hpp"
#include "lue/string.hpp"
#include <cassert>
#include <format>
#include <ranges>


namespace lue::gdal {
    namespace {


        template<typename Element>
        auto compare_values(
            Raster::Band& band1,
            Raster::Band& band2,
            bool const no_data_value_valid,
            Element const no_data_value,
            std::vector<std::string>& differences) -> void
        {
            auto compare_elements = [no_data_value_valid, no_data_value](
                                        std::vector<Element> const& values1,
                                        std::vector<Element> const& values2,
                                        Count nr_valid_cells)
            {
                bool result{false};

                if constexpr (std::is_floating_point_v<Element>)
                {
                    if (no_data_value_valid && std::isnan(no_data_value))
                    {
                        result = std::equal(
                            values1.begin(),
                            values1.begin() + nr_valid_cells,
                            values2.begin(),
                            [](Element const value1, Element const value2)
                            { return (std::isnan(value1) && std::isnan(value2)) || (value1 == value2); });
                    }
                    else
                    {
                        result = std::equal(
                            values1.begin(),
                            values1.begin() + nr_valid_cells,
                            values2.begin(),
                            [](Element const value1, Element const value2) { return value1 == value2; });
                    }
                }
                else
                {
                    return std::equal(
                        values1.begin(),
                        values1.begin() + nr_valid_cells,
                        values2.begin(),
                        [](Element const value1, Element const value2) { return value1 == value2; });
                }

                return result;
            };


            // Read blocks of data and compare the values
            auto const shape{band1.shape()};
            auto const block_shape{band1.block_shape()};
            Blocks const blocks{shape, block_shape};

            std::vector<Element> values1(nr_elements(block_shape));
            std::vector<Element> values2(nr_elements(block_shape));

            auto const [nr_blocks_y, nr_blocks_x] = blocks.shape_in_blocks();
            bool different_values_found{false};

            for (gdal::Count block_y = 0; block_y < nr_blocks_y && !different_values_found; ++block_y)
            {
                for (gdal::Count block_x = 0; block_x < nr_blocks_x && !different_values_found; ++block_x)
                {
                    auto const nr_valid_cells = nr_elements(blocks.shape_in_valid_cells({block_x, block_y}));

                    band1.read_block({block_y, block_x}, values1.data());
                    band2.read_block({block_y, block_x}, values2.data());

                    bool values_are_equal = compare_elements(values1, values2, nr_valid_cells);

                    if (!values_are_equal)
                    {
                        differences.emplace_back("different cell values");
                        different_values_found = true;
                    }
                }
            }
        }


        template<typename Element>
        auto compare_raster_bands(
            Raster::Band& band1, Raster::Band& band2, std::vector<std::string>& differences) -> void
        {
            auto const shape1 = band1.shape();
            auto const shape2 = band2.shape();
            bool const shapes_equal = shape1 == shape2;

            if (!shapes_equal)
            {
                differences.emplace_back(
                    std::format("different shapes: ({}) != ({})", join(shape1, ", "), join(shape2, ", ")));
            }

            auto const data_type1 = band1.data_type();
            auto const data_type2 = band2.data_type();
            bool const data_types_equal = data_type1 == data_type2;

            if (!data_types_equal)
            {
                differences.emplace_back(std::format(
                    "different data types: {} != {}", as_string(data_type1), as_string(data_type2)));
            }


            auto const [no_data_value1, no_data_valid1] = band1.no_data_value<Element>();
            auto const [no_data_value2, no_data_valid2] = band2.no_data_value<Element>();
            bool const both_no_data_values_valid = no_data_valid1 && no_data_valid2;
            bool const both_no_data_values_invalid = !no_data_valid1 && !no_data_valid2;
            bool const both_no_data_values_valid_or_invalid =
                both_no_data_values_valid || both_no_data_values_invalid;

            if (!both_no_data_values_valid_or_invalid)
            {
                differences.emplace_back(
                    std::format("different no-data validity: {} != {}", no_data_valid1, no_data_valid2));
            }

            bool const no_data_values_equal = [](Element const no_data_value1, Element const no_data_value2)
            {
                bool result{false};

                if constexpr (std::is_floating_point_v<Element>)
                {
                    result = (std::isnan(no_data_value1) && std::isnan(no_data_value2)) ||
                             (no_data_value1 == no_data_value2);
                }
                else
                {
                    result = no_data_value1 == no_data_value2;
                }

                return result;
            }(no_data_value1, no_data_value2);

            if (both_no_data_values_valid && !no_data_values_equal)
            {
                differences.emplace_back(
                    std::format("different no-data values: {} != {}", no_data_value1, no_data_value2));
            }

            bool const no_data_settings_equal = both_no_data_values_invalid || no_data_values_equal;

            // Block shape can differ. This is a format dependant thing.

            if (shapes_equal && data_types_equal && no_data_settings_equal)
            {
                compare_values<Element>(band1, band2, both_no_data_values_valid, no_data_value1, differences);
            }
        }


        template<typename Element>
        auto compare_raster_bands(Raster::Band& band1, Raster::Band& band2) -> std::vector<std::string>
        {
            std::vector<std::string> differences{};

            compare_raster_bands<Element>(band1, band2, differences);

            return differences;
        }

    }  // Anonymous namespace


    /*!
        @brief      Compare two raster datasets
        @return     A collection of descriptions of differences, if any. If this collection is
                    empty, the rasters contain exactly the same information.
    */
    auto compare_rasters(std::string const& dataset_name1, std::string const& dataset_name2)
        -> std::vector<std::string>
    {
        std::vector<std::string> differences{};

        auto dataset1 = try_open_dataset(dataset_name1, GDALAccess::GA_ReadOnly);
        auto dataset2 = try_open_dataset(dataset_name2, GDALAccess::GA_ReadOnly);

        // Existence
        if (!dataset1 || !dataset2)
        {
            differences.emplace_back(std::format(
                "One or both datasets don't exist: {}: {} / {}: {}",
                dataset_name1,
                static_cast<bool>(dataset1),
                dataset_name2,
                static_cast<bool>(dataset2)));
            return differences;
        }

        Raster raster1{dataset1};
        Raster raster2{dataset2};

        auto const shape1 = raster1.shape();
        auto const shape2 = raster2.shape();
        bool const shapes_equal = shape1 == shape2;

        if (!shapes_equal)
        {
            differences.emplace_back(
                std::format("different shapes: ({}) != ({})", join(shape1, ", "), join(shape2, ", ")));
        }

        auto const data_type1 = raster1.data_type();
        auto const data_type2 = raster2.data_type();
        bool const data_types_equal = data_type1 == data_type2;

        if (!data_types_equal)
        {
            differences.emplace_back(
                std::format("different data types: {} != {}", as_string(data_type1), as_string(data_type2)));
        }

        auto const nr_bands1 = raster1.nr_bands();
        auto const nr_bands2 = raster2.nr_bands();
        bool const nr_bands_equal = nr_bands1 == nr_bands2;

        if (!nr_bands_equal)
        {
            differences.emplace_back(
                std::format("different number of bands: {} != {}", nr_bands1, nr_bands2));
        }

        auto const geo_transform1 = raster1.geo_transform();
        auto const geo_transform2 = raster2.geo_transform();
        bool const geo_transforms_equal = geo_transform1 == geo_transform2;

        if (!geo_transforms_equal)
        {
            differences.emplace_back(std::format(
                "different geo-transformations: [{}] != [{}]",
                join(geo_transform1, ", "),
                join(geo_transform2, ", ")));
        }

        // TODO SRS
        bool const srs_equal{true};

        if (shapes_equal && data_types_equal && geo_transforms_equal && nr_bands_equal && srs_equal)
        {
            for (Count band_nr = 1; band_nr <= nr_bands1; ++band_nr)
            {
                auto band1 = raster1.band(band_nr);
                auto band2 = raster2.band(band_nr);

                switch (data_type1)
                {
                    case GDT_Byte:
                    {
                        compare_raster_bands<std::uint8_t>(band1, band2, differences);
                        break;
                    }
#if LUE_GDAL_SUPPORTS_8BIT_SIGNED_INTEGERS
                    case GDT_Int8:
                    {
                        compare_raster_bands<std::int8_t>(band1, band2, differences);
                        break;
                    }
#endif
                    case GDT_UInt16:
                    {
                        compare_raster_bands<std::uint16_t>(band1, band2, differences);
                        break;
                    }
                    case GDT_Int16:
                    {
                        compare_raster_bands<std::int16_t>(band1, band2, differences);
                        break;
                    }
                    case GDT_UInt32:
                    {
                        compare_raster_bands<std::uint32_t>(band1, band2, differences);
                        break;
                    }
                    case GDT_Int32:
                    {
                        compare_raster_bands<std::int32_t>(band1, band2, differences);
                        break;
                    }
#if LUE_GDAL_SUPPORTS_64BIT_INTEGERS
                    case GDT_UInt64:
                    {
                        compare_raster_bands<std::uint64_t>(band1, band2, differences);
                        break;
                    }
                    case GDT_Int64:
                    {
                        compare_raster_bands<std::int64_t>(band1, band2, differences);
                        break;
                    }
#endif
                    case GDT_Float32:
                    {
                        compare_raster_bands<float>(band1, band2, differences);
                        break;
                    }
                    case GDT_Float64:
                    {
                        compare_raster_bands<double>(band1, band2, differences);
                        break;
                    }
                    default:
                    {
                        differences.emplace_back(
                            std::format("Unsupported GDAL data type: {}", as_string(data_type1)));
                    }
                }
            }
        }

        return differences;
    }

}  // namespace lue::gdal

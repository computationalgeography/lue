#include "lue/utility/lue_translate/gdal.h"
#include "lue/utility/lue_translate/file_stack.h"
#include "lue/utility/lue_translate/gdal_block.h"
#include "lue/utility/lue_translate/progress_indicator.h"
#include "lue/utility/lue_translate/time/time_period.h"
#include "lue/cxx_api.h"
#include <ogrsf_frmts.h>
#include <cassert>
#include <cmath>
#include <iostream>
#include <numeric>


namespace lue {
namespace {

std::tuple<hid_t, hid_t> gdal_type_to_hdf5_type(
    GDALDataType const datatype)
{
    hid_t file_type_id = -1;
    hid_t memory_type_id = -1;

    switch(datatype) {
        case GDT_Byte: {
            file_type_id = H5T_STD_U8LE;
            memory_type_id = H5T_NATIVE_UINT8;
            break;
        }
        case GDT_UInt16: {
            file_type_id = H5T_STD_U16LE;
            memory_type_id = H5T_NATIVE_UINT16;
            break;
        }
        case GDT_Int16: {
            file_type_id = H5T_STD_I16LE;
            memory_type_id = H5T_NATIVE_INT16;
            break;
        }
        case GDT_UInt32: {
            file_type_id = H5T_STD_U32LE;
            memory_type_id = H5T_NATIVE_UINT32;
            break;
        }
        case GDT_Int32: {
            file_type_id = H5T_STD_I32LE;
            memory_type_id = H5T_NATIVE_INT32;
            break;
        }
        case GDT_Float32: {
            file_type_id = H5T_IEEE_F32LE;
            memory_type_id = H5T_NATIVE_FLOAT;
            break;
        }
        case GDT_Float64: {
            file_type_id = H5T_IEEE_F64LE;
            memory_type_id = H5T_NATIVE_DOUBLE;
            break;
        }
        default: {
            assert(false);
        }
    }

    return std::make_tuple(file_type_id, memory_type_id);
}


struct GDALFeatureDeleter
{
    void operator()(::OGRFeature* feature) const
    {
        if(feature) {
            ::OGRFeature::DestroyFeature(feature);
        }
    }
};

using GDALFeaturePtr = std::unique_ptr<::OGRFeature, GDALFeatureDeleter>;


std::string property_name(
    int band_id)
{
    return "band_" + std::to_string(band_id);
}


class GDALDomain
{

public:

    GDALDomain(
        std::string const& wkt)

        : wkt(wkt)

    {
    }

    virtual ~GDALDomain()=default;

    std::string const wkt;

};


class GDALFeatureDomain:
    public GDALDomain
{

public:

    GDALFeatureDomain(
        std::string const& wkt,
        int const nr_layers)

        : GDALDomain(wkt),
          nr_layers{nr_layers}

    {
    }

    int const nr_layers;

};


class GDALRasterDomain:
    public GDALDomain
{

public:

    GDALRasterDomain(
        std::string const& wkt,
        int const nr_bands,
        double const west,
        double const north,
        double const east,
        double const south,
        int const nr_rows,
        int const nr_cols)

        : GDALDomain(wkt),
          nr_bands{nr_bands},
          west{west},
          north{north},
          east{east},
          south{south},
          nr_rows{nr_rows},
          nr_cols{nr_cols}

    {
    }

    int nr_bands;

    double const west;
    double const north;
    double const east;
    double const south;

    int const nr_rows;
    int const nr_cols;

};


// GDALFeatureDomain gdal_feature_domain(
//     GDALDataset& dataset)
// {
//     int const nr_layers = dataset.GetLayerCount();
//     assert(nr_layers >= 0);
//     std::string const projection_wkt = dataset.GetProjectionRef();
// 
//     return GDALFeatureDomain(projection_wkt, nr_layers);
// }


GDALRasterDomain gdal_raster_domain(
    GDALDataset& dataset)
{
    int const nr_rows = dataset.GetRasterYSize();
    assert(nr_rows > 0);
    int const nr_cols = dataset.GetRasterXSize();
    assert(nr_cols > 0);
    int const nr_bands = dataset.GetRasterCount();
    assert(nr_bands >= 0);
    // assert(nr_bands == dataset.GetLayerCount());
    std::string const projection_wkt = dataset.GetProjectionRef();

    double geo_transform[6];
    dataset.GetGeoTransform(geo_transform);

    double const west = geo_transform[0];
    double const north = geo_transform[3];
    double const cell_width = geo_transform[1];
    assert(cell_width > 0.0);
    double const cell_height = std::abs(geo_transform[5]);
    assert(cell_height > 0.0);
    double const east = west + (nr_cols * cell_width);
    double const south = north - (nr_rows * cell_height);

    return GDALRasterDomain(projection_wkt, nr_bands,
        west, north, east, south,
        nr_rows, nr_cols);
}


template<
    typename T>
void copy_band(
    GDALRasterBand& raster_band,
    GDALBlock const& blocks,
    Array&& array)
{
    ProgressIndicator progress_indicator(std::cout,
        "Copying blocks", blocks.nr_blocks());

    // Import current band into current property value.
    std::vector<T> values(blocks.block_size());
    size_t nr_valid_cells_x;
    size_t nr_valid_cells_y;
    size_t current_block = 0;

    for(size_t block_y = 0; block_y < blocks.nr_blocks_y();
            ++block_y) {
        for(size_t block_x = 0; block_x < blocks.nr_blocks_x();
                ++block_x) {
            auto cpl_status = raster_band.ReadBlock(block_x, block_y,
                values.data());
            assert(cpl_status == CE_None);  // TODO

            std::tie(nr_valid_cells_x, nr_valid_cells_y) =
                blocks.nr_valid_cells(block_x, block_y);

            std::vector<extent_t> extents = {
                nr_valid_cells_x * nr_valid_cells_y
            };
            auto memory_dataspace = hdf5::create_dataspace(extents);

            std::vector<extent_t> start = {
                block_y * blocks.block_size_y(),
                block_x * blocks.block_size_x()
            };
            std::vector<extent_t> stride = {
                1,
                1
            };
            std::vector<extent_t> count = {
                nr_valid_cells_y,
                nr_valid_cells_x
            };

            array.write(memory_dataspace, start, count, stride,
                values.data());

            progress_indicator.update_progress(++current_block);
        }
    }
}


auto create_space_discretization_property(
    time::omnipresent::PropertySet& o_property_set,
    GDALRasterDomain const& gdal_domain,
    count_t const nr_items)
{
    // Discretization property.
    // Domain of discretization is the same as of the band properties so
    // they can all be part of the same property set. We need to set up
    // a link between each band property value and the discretization
    // property.
    auto const file_type_id = H5T_STD_U64LE;
    auto const memory_type_id = H5T_NATIVE_UINT64;
    Shape const shape{ 2 };
    Chunks const chunks{ 2 };

    auto& discretization = o_property_set.add_property(
        "space discretization", file_type_id, memory_type_id, shape,
        chunks);

    {
        // Per item a 1D array of <rank> values representing the size of
        // the dimensions.
        uint64_t values[2] = {
            static_cast<uint64_t>(gdal_domain.nr_rows),
            static_cast<uint64_t>(gdal_domain.nr_cols)
        };

        auto& item = discretization.reserve_items(nr_items);
        item.write({nr_items, 2}, values);
    }

    discretization.group().attributes().write<std::string>(
        "lue_discretization_type", space_discretization_type_to_string(
            SpaceDiscretizationType::cartesian_grid));

    return std::move(discretization);
}


auto create_time_discretization_property(
    time::shared_constant::PropertySet /* sc_property_set */,
    // GDALRasterDomain const& gdal_domain,
    count_t const /* nr_items */)
{



    // // Discretization property.
    // // Domain of discretization is the same as of the band properties so
    // // they can all be part of the same property set. We need to set up
    // // a link between each band property value and the discretization
    // // property.
    // auto const file_type_id = H5T_STD_U64LE;
    // auto const memory_type_id = H5T_NATIVE_UINT64;
    // Shape const shape{ 2 };
    // Chunks const chunks{ 2 };

    // auto& discretization = o_property_set.add_property(
    //     "space discretization", file_type_id, memory_type_id, shape,
    //     chunks);

    // {
    //     // Per item a 1D array of <rank> values representing the size of
    //     // the dimensions.
    //     uint64_t values[2] = {
    //         static_cast<uint64_t>(gdal_domain.nr_rows),
    //         static_cast<uint64_t>(gdal_domain.nr_cols)
    //     };

    //     auto& item = discretization.reserve_items(nr_items);
    //     item.write({nr_items, 2}, values);
    // }

    // discretization.group().attributes().write<std::string>(
    //     "lue_discretization_type", space_discretization_type_to_string(
    //         SpaceDiscretizationType::cartesian_grid));

    // return std::move(discretization);
}

}  // anonymous namespace


// Try to open dataset using GDAL. Read-only is fine.
// Return pointer to dataset if successful.
GDALDatasetPtr try_open_gdal_raster_dataset_for_read(
    std::string const& input_dataset_name)
{
    CPLPushErrorHandler(CPLQuietErrorHandler);

    auto result = GDALDatasetPtr(
        static_cast<::GDALDataset*>(::GDALOpenEx(input_dataset_name.c_str(),
            GDAL_OF_READONLY | GDAL_OF_RASTER, nullptr, nullptr, nullptr)),
        GDALDatasetDeleter{});

    CPLPopErrorHandler();

    return result;
}


// Try to open dataset using GDAL. Read-only is fine.
// Return pointer to dataset if successful.
GDALDatasetPtr try_open_gdal_feature_dataset_for_read(
    std::string const& input_dataset_name)
{
    CPLPushErrorHandler(CPLQuietErrorHandler);

    auto result = GDALDatasetPtr(
        static_cast<::GDALDataset*>(::GDALOpenEx(input_dataset_name.c_str(),
            GDAL_OF_READONLY | GDAL_OF_VECTOR, nullptr, nullptr, nullptr)),
        GDALDatasetDeleter{});

    CPLPopErrorHandler();

    return result;
}


// Try to open stack using gdal. Read-only is fine.
// Return pointer to dataset if successful.
GDALDatasetPtr try_open_gdal_raster_stack_for_read(
    std::string const& input_dataset_name)
{
    GDALDatasetPtr result;

    CPLPushErrorHandler(CPLQuietErrorHandler);

    std::string pathname{stack_slice_pathname(input_dataset_name,
        raster_extensions)};

    if(!pathname.empty()) {
        result = try_open_gdal_raster_dataset_for_read(pathname);
    }

    CPLPopErrorHandler();

    return result;
}


GDALDatasetPtr try_open_gdal_feature_stack_for_read(
    std::string const& input_dataset_name)
{
    GDALDatasetPtr result;

    CPLPushErrorHandler(CPLQuietErrorHandler);

    std::string pathname{stack_slice_pathname(input_dataset_name,
        feature_extensions)};

    if(!pathname.empty()) {
        result = try_open_gdal_feature_dataset_for_read(pathname);
    }

    CPLPopErrorHandler();

    return result;
}


GDALDatasetPtr try_open_gdal_track_dataset_for_read(
    std::string const& input_dataset_name)
{
    CPLPushErrorHandler(CPLQuietErrorHandler);

    char const* driver_names[] = {
        "GPX",
        nullptr
    };

    auto result = GDALDatasetPtr(
        static_cast<::GDALDataset*>(::GDALOpenEx(input_dataset_name.c_str(),
            GDAL_OF_READONLY | GDAL_OF_VECTOR, driver_names, nullptr,
            nullptr)),
        GDALDatasetDeleter{});

    CPLPopErrorHandler();

    return result;
}


// Template of function importing one raster or a stack of rasters.

// - Input:
//     - Determine space domain of input
//     - Determine ѕpace discretization of input
//
// - Output:
//     - Create LUE dataset
//     - Add phenomenon to dataset
//     - Add property set to phenomenon
//     - Write time domain
//     - Write space domain
//     - Create and write discretization property to property set
//     - For each band, create a property
//     - For each band, import source band to destination property value



void translate_gdal_raster_to_lue(
    ::GDALDataset& gdal_dataset,
    std::string const& gdal_dataset_name,
    std::string const& lue_dataset_name)
{
    // Read raster layers using gdal and write them to a new LUE dataset.
    // Existing information in the target dataset is deleted.

    GDALRasterDomain gdal_domain = gdal_raster_domain(gdal_dataset);

    // Write information to LUE dataset. ---------------------------------------
    // Open LUE dataset for writing, overwriting existing contents.
    // TODO Add log message to dataset:
    //     <timepoint> Created by lue_translate <version> using
    //     these arguments: ...
    auto lue_dataset = create_dataset(lue_dataset_name);


    // Add phenomenon to the LUE dataset. Name it after the GDAL dataset.
    auto const phenomenon_name =
        boost::filesystem::path(gdal_dataset_name).stem().string();
    auto& phenomenon = lue_dataset.add_phenomenon(phenomenon_name);


    // Add property set to the phenomenon. This one set will contain all
    // raster layers from the GDAL dataset.
    auto const property_set_name = "areas";
    PropertySetConfiguration property_set_configuration(
        SizeOfItemCollectionType::constant_size);
    DomainConfiguration domain_configuration{
        SpaceDomainConfiguration(SpaceDomainItemType::box)};
    auto& areas = phenomenon.add_property_set(property_set_name,
        property_set_configuration, domain_configuration);

    count_t const nr_items = 1;
    rank_t const rank = 2;

    time::omnipresent::PropertySet o_property_set(areas);

    // Write property ids.
    {
        item_t const item_id[nr_items] = { 0 };

        auto& item = o_property_set.reserve_items(nr_items);
        item.write(nr_items, item_id);
    }


    // Write space box.
    {
        double const box_coordinate[nr_items * 2 * rank] = {
            gdal_domain.west,
            gdal_domain.south,
            gdal_domain.east,
            gdal_domain.north
        };

        space::stationary::SpaceBoxDomain s_space_domain(
            areas.domain().space_domain());
        auto& boxes = s_space_domain.reserve_items(nr_items);
        boxes.write({nr_items, 2 * rank}, box_coordinate);
    }


    auto space_discretization = create_space_discretization_property(
        o_property_set, gdal_domain, nr_items);


    {
        extent_t shapes[rank] = {
            static_cast<extent_t>(gdal_domain.nr_rows),
            static_cast<extent_t>(gdal_domain.nr_cols)
        };


        // Use one row as the unit of chunking.
        // TODO Does this make sense?
        // Chunks const chunks{ 1, static_cast<extent_t>(gdal_domain.nr_cols) };

        hid_t file_type_id;
        hid_t memory_type_id;

        // Import all raster bands.
        for(int b = 1; b <= gdal_domain.nr_bands; ++b) {

            GDALRasterBand* raster_band = gdal_dataset.GetRasterBand(b);
            assert(raster_band);
            GDALDataType datatype = raster_band->GetRasterDataType();

            std::tie(file_type_id, memory_type_id) = gdal_type_to_hdf5_type(
                datatype);

            auto& property = o_property_set.add_property(property_name(b),
                file_type_id, rank);
            property.group().value().create_soft_link(
                space_discretization.id(), "lue_space_discretization");
            auto& item = property.reserve_items(nr_items, shapes);


            int block_size_x, block_size_y;
            raster_band->GetBlockSize(&block_size_x, &block_size_y);
            GDALBlock blocks(gdal_domain.nr_cols, gdal_domain.nr_rows,
                block_size_x, block_size_y);

            switch(datatype) {
                case GDT_Byte: {
                    copy_band<uint8_t>(*raster_band, blocks, item[0]);
                    break;
                }
                case GDT_UInt16: {
                    copy_band<uint16_t>(*raster_band, blocks, item[0]);
                    break;
                }
                case GDT_Int16: {
                    copy_band<int16_t>(*raster_band, blocks, item[0]);
                    break;
                }
                case GDT_UInt32: {
                    copy_band<uint32_t>(*raster_band, blocks, item[0]);
                    break;
                }
                case GDT_Int32: {
                    copy_band<int32_t>(*raster_band, blocks, item[0]);
                    break;
                }
                case GDT_Float32: {
                    copy_band<float>(*raster_band, blocks, item[0]);
                    break;
                }
                case GDT_Float64: {
                    copy_band<double>(*raster_band, blocks, item[0]);
                    break;
                }
                default: {
                    assert(false);
                    break;
                }
            }
        }
    }


    // Write all metadata items in the GDAL dataset to the LUE dataset.
    // TODO ...
}


// dataset GDAL dataset representing a slice from the stack
// gdal_dataset_name Name of dataset
// start_time_point Time point of first slice
// slice_duration Duration of time step
// lue_dataset_name Name of dataset to write
void translate_gdal_raster_stack_to_lue(
    ::GDALDataset& gdal_dataset,
    std::string const& gdal_dataset_name,
    MonthTimePoint const& start_time_point,
    MonthDuration const& slice_duration,
    std::string const& lue_dataset_name)
{
    GDALRasterDomain gdal_domain = gdal_raster_domain(gdal_dataset);


    // Create LUE dataset.
    auto lue_dataset = create_dataset(lue_dataset_name);

    // Add phenomenon to the LUE dataset. Name it after the GDAL dataset.
    auto const phenomenon_name =
        boost::filesystem::path(gdal_dataset_name).stem().string();
    auto& phenomenon = lue_dataset.add_phenomenon(phenomenon_name);

    // Add property set to the phenomenon. This one set will contain all
    // raster layers from the GDAL dataset.
    std::string property_set_name = "areas";
    PropertySetConfiguration property_set_configuration(
        SizeOfItemCollectionType::constant_size);
    DomainConfiguration domain_configuration(
        TimeDomainConfiguration(TimeDomainType::shared,
            TimeDomainItemType::period),
        SpaceDomainConfiguration(SpaceDomainItemType::box));
    auto& areas = phenomenon.add_property_set(property_set_name,
        property_set_configuration, domain_configuration);

    count_t const nr_items = 1;
    rank_t const rank = 2;

    time::shared_constant::PropertySet sc_areas(areas);

    // Write property ids.
    item_t const item_id[nr_items] = { 0 };

    {
        auto& item = sc_areas.reserve_items(nr_items);
        item.write(nr_items, item_id);
    }


    // Write time period.
    {
        // The begin time point is passed in. The duration of the period is
        // determined by the last slice available.
        auto const file_extension = gdal_slice_filename_extension(
            gdal_dataset.GetFileList(), raster_extensions);
        std::set<size_t> slice_indices = stack_slice_indices(gdal_dataset_name,
            file_extension);
        assert(!slice_indices.empty());

        size_t const first_slice = *slice_indices.begin();
        size_t const last_slice = *(--slice_indices.end());
        size_t const nr_slices = last_slice - first_slice + 1;

        auto const stack_duration = nr_slices * slice_duration;

        count_t const nr_time_domain_items = 1;
        int32_t const period_coordinates[nr_time_domain_items * 3] = {
            start_time_point.year(),
            start_time_point.month(),
            static_cast<int32_t>(stack_duration.count())
        };

        time::shared::TimePeriodDomain s_time_domain(
            areas.domain().time_domain());

        auto& periods = s_time_domain.reserve_items(nr_time_domain_items);
        periods.write({nr_time_domain_items, 3}, period_coordinates);
    }


    // Write space box.
    {
        double const box_coordinate[nr_items * 2 * rank] = {
            gdal_domain.west,
            gdal_domain.south,
            gdal_domain.east,
            gdal_domain.north
        };

        space::stationary::SpaceBoxDomain s_space_domain(
            areas.domain().space_domain());

        auto& boxes = s_space_domain.reserve_items(nr_items);
        boxes.write({nr_items, 2 * rank}, box_coordinate);
    }




    // Flow when decoding a property's value:
    // - If there is a time discretization link, open de property set
    //   it points to


    // Shared time domain:
    // - All domain items are shared by all items.
    // - Properties (time discretization) of the domain are shared by
    //   all items.


    // Time discretization.
    // - Per domain item a different discretization.
    // - Per item the same discretization.


    // The property set to store discretization information for the time
    // domain items has this layout:
    // - time domain
    // - space domain


    // auto time_discretization = create_time_discretization_property(sc_areas,
    //     /* gdal_domain, */ nr_items);





    property_set_name = "globals";
    auto& globals = phenomenon.add_property_set(property_set_name);
    time::omnipresent::PropertySet o_globals(globals);

    {
        auto& item = o_globals.reserve_items(nr_items);
        item.write(nr_items, item_id);
    }

    auto space_discretization = create_space_discretization_property(o_globals,
        gdal_domain, nr_items);




    // property.group().create_soft_link(space_discretization.id(),
    //     "lue_space_discretization");








    // // Discretization property.
    // // Domain of discretization is the same as of the band properties so
    // // they can all be part of the same property set. We need to set up
    // // a link between each band property value and the discretization
    // // property.
    // auto const file_type_id = H5T_STD_U64LE;
    // auto const memory_type_id = H5T_NATIVE_UINT64;
    // Shape const shape{ 2 };
    // Chunks const chunks{ 2 };

    // auto& discretization = o_globals.add_property(
    //     "space discretization", file_type_id, memory_type_id, shape,
    //     chunks);

    // {
    //     // Per item a 1D array of <rank> values representing the size of
    //     // the dimensions.
    //     uint64_t values[2] = {
    //         static_cast<uint64_t>(gdal_domain.nr_rows),
    //         static_cast<uint64_t>(gdal_domain.nr_cols)
    //     };

    //     auto& item = discretization.reserve_items(nr_items);
    //     item.write({nr_items, 2}, values);
    // }












//     auto& domain = property_set.domain();
//     hsize_t const nr_items = 1;
// 
//     herr_t status;
// 
//     // domain.attributes().write<std::string>("time_domain_type", "located");
// 
//     // Constant collection of shared domain items.
// 
// 
//     // Item collection.
//     {
//         hdf5::Identifier item_collection(create_item_collection(domain.id(),
//                 LUE_STD_ITEM, nr_items), close_item_collection);
//         int const items[nr_items] = { 0 };
//         status = write_item_collection(item_collection,
//             LUE_NATIVE_ITEM, items);
//         check_status(lue_dataset_name, status, "write item collection");
//     }
// 
// 
//     // Time domain contains a single time extent. It is defined by the
//     // begin time point and a duration.
// 
//     // In case the range of slices is continuous (no holes), we only
//     // need to store a single time period. Otherwise we have to treat the
//     // holes as periods with no-data, or store multiple periods. Probably
//     // support no-data periods.
// 
//     // The begin time point is passed in. The duration of the period is
//     // determined by the last slice available.
// 
//     auto const file_extension = gdal_slice_filename_extension(
//         gdal_dataset.GetFileList(), raster_extensions);
//     std::set<size_t> slice_indices = stack_slice_indices(gdal_dataset_name,
//         file_extension);
//     assert(!slice_indices.empty());
// 
//     size_t const first_slice = *slice_indices.begin();
//     size_t const last_slice = *(--slice_indices.end());
//     size_t const nr_slices = last_slice - first_slice + 1;
// 
//     auto const stack_duration = nr_slices * slice_duration;
// 
//     // First slice starts at the time point passed in. It ends after one
//     // slice durations has passed.
//     // {time_point, stack_duration}
// 
// 
//     // Time point types.
//     auto const year_file_type_id = H5T_STD_I32LE;
//     auto const month_file_type_id = H5T_STD_I32LE;
//     auto const year_memory_type_id = H5T_NATIVE_INT32;
//     auto const month_memory_type_id = H5T_NATIVE_INT32;
// 
//     hdf5::Identifier const time_point_file_type(
//         create_month_time_point_file_datatype(year_file_type_id,
//             month_file_type_id), H5Tclose);
//     hdf5::Identifier const time_point_memory_type(
//         create_month_time_point_memory_datatype(year_memory_type_id,
//             month_memory_type_id), H5Tclose);
// 
// 
//     // Time duration types.
//     auto const count_file_type_id = H5T_STD_I32LE;
//     auto const count_memory_type_id = H5T_NATIVE_INT32;
// 
//     hdf5::Identifier const time_duration_file_type(
//         create_month_time_duration_file_datatype(count_file_type_id),
//             H5Tclose);
//     hdf5::Identifier const time_duration_memory_type(
//         create_month_time_duration_memory_datatype(count_memory_type_id),
//             H5Tclose);
// 
// 
//     // Time period types.
//     hdf5::Identifier const time_domain_item_file_type(
//         create_time_period_file_datatype(time_point_file_type,
//             time_duration_file_type), H5Tclose);
//     hdf5::Identifier const time_domain_item_memory_type(
//         create_time_period_memory_datatype(time_point_memory_type,
//             time_duration_memory_type), H5Tclose);
// 
// 
//     // Time domain.
//     hdf5::Identifier const time_domain(create_shared_time_domain(domain.id(),
//         time_domain_item_file_type), close_shared_time_domain);
// 
//     status = reserve_time_domain_items_space(time_domain, 1);
//     check_status(lue_dataset_name, status, "reserve time domain items space");
// 
// 
//     // TODO Is the layout of TimePeriod guaranteed to be
//     // {
//     //     time_point
//     //     time_duration
//     // }
//     // This is assumed by create_time_period_xxx_datatype().
//     TimePeriod<MonthTimePoint, MonthDuration> time_period(start_time_point,
//         stack_duration);
//     status = write_time_domain_items(time_domain,
//         time_domain_item_memory_type, &time_period, 1);
//     check_status(lue_dataset_name, status, "write time domain items");
// 
// 
//     // Space domain.
//     // Domain items are space boxes.
//     {
//         // domain.attributes().write<std::string>("space_domain_type", "located");
// 
//         int const rank = 2;
// 
//         hdf5::Identifier space_domain_item_file_type(
//             create_space_box_file_datatype(rank, H5T_IEEE_F64LE), H5Tclose);
//         hdf5::Identifier space_domain_item_memory_type(
//             create_space_box_memory_datatype(rank, H5T_NATIVE_DOUBLE),
//             H5Tclose);
//         hdf5::Identifier space_domain(create_simple_space_domain(domain.id(),
//             space_domain_item_file_type, nr_items), close_simple_space_domain);
//         // TODO test space_domain.id(), or raise exception
// 
//         std::vector<BoxFloat64> space_domain_items(nr_items);
//         space_domain_items[0].min.x = gdal_domain.west;
//         space_domain_items[0].min.y = gdal_domain.south;
//         space_domain_items[0].max.x = gdal_domain.east;
//         space_domain_items[0].max.y = gdal_domain.north;
// 
//         // hdf5::Identifier item_dataset(hdf5_open_dataset(space_domain.id(),
//         //     "item"), hdf5_close_dataset);
//         // TODO test space_domain.id(), or raise exception
// 
//         status = write_simple_space_domain_items(space_domain,
//             space_domain_item_memory_type, space_domain_items.data());
//         check_status(lue_dataset_name, status, "write space domain items");
//     }
// 
// 
//     // Discretization property.
//     // Domain of time and space discretizations is the same as of the
//     // band properties so they can all be part of the same property
//     // set. We need to set up a link between each band property value and
//     // the time and space discretization properties.
// 
// 
//     auto& time_discretization = property_set.add_property(
//         "time discretization");
//     check_id(lue_dataset_name, time_discretization, "create property");
// 
//     {
//         // Per item a 1D array of <rank> values representing the size of
//         // the dimensions.
// 
//         int const rank = 1;
//         hsize_t const shape[rank] = { 1 };
//         hsize_t const chunks[rank] = { 1 };
// 
//         hdf5::Identifier value(create_value(time_discretization.id(),
//             H5T_STD_U64LE, rank, shape, chunks), close_value);
//         check_id(lue_dataset_name, value, "create value");
// 
//         uint64_t values[1] = { static_cast<uint64_t>(nr_slices) };
// 
//         herr_t status = write_value(value, H5T_NATIVE_UINT64,
//             values, nr_items);
//         check_status(lue_dataset_name, status, "write value");
//     }
// 
// 
//     auto& space_discretization = property_set.add_property(
//         "space discretization");
//     check_id(lue_dataset_name, space_discretization, "create property");
// 
//     {
//         // Per item a 1D array of <rank> values representing the size of
//         // the dimensions.
// 
//         int const rank = 1;
//         hsize_t const shape[rank] = { 2 };
//         hsize_t const chunks[rank] = { 2 };
// 
//         hdf5::Identifier value(create_value(space_discretization.id(),
//             H5T_STD_U64LE, rank, shape, chunks), close_value);
//         check_id(lue_dataset_name, value, "create value");
// 
//         uint64_t values[2] = { static_cast<uint64_t>(gdal_domain.nr_rows),
//             static_cast<uint64_t>(gdal_domain.nr_cols) };
// 
//         herr_t status = write_value(value, H5T_NATIVE_UINT64,
//             values, nr_items);
//         check_status(lue_dataset_name, status, "write value");
//     }
// 
//     {
//         int const value_rank = 2;
//         hsize_t const shape[value_rank] = {
//            static_cast<hsize_t>(gdal_domain.nr_rows),
//            static_cast<hsize_t>(gdal_domain.nr_cols)
//         };
// 
//         // Use one row as the unit of chunking.
//         // TODO Does this make sense?
//         hsize_t const chunks[value_rank] = {
//            1, static_cast<hsize_t>(gdal_domain.nr_cols) };
// 
//         // There is only one item.
//         hsize_t const* item_shapes = &shape[0];
//         hsize_t const* item_chunks = &chunks[0];
// 
//         // Create a property for each layer in the GDAL raster dataset.
//         for(int b = 1; b <= gdal_domain.nr_bands; ++b) {
//             auto& property = create_band_property(lue_dataset_name,
//                 property_set, b, time_discretization,
//                 space_discretization);
// 
//             // Value API:
//             // - constant collection of shared domain items
//             // - variable size per item
//             // - constant size through time
// 
//             // TODO
//             // Switch on value type.
// 
//             hdf5::Identifier value(create_t_n_value(property.id(),
//                 H5T_IEEE_F32LE, value_rank, &item_shapes, &item_chunks,
//                 nr_items), close_t_n_value);
//             check_id(lue_dataset_name, value, "create value");
//         }
// 
// 
//         boost::filesystem::path input_dataset_path;
//         GDALDatasetPtr gdal_dataset_ptr;
//         size_t const nr_bands_to_copy = nr_slices * gdal_domain.nr_bands;
//         size_t band_to_copy = 0;
// 
// 
//         ProgressIndicator progress_indicator(std::cout, "copying bands",
//             nr_bands_to_copy);
// 
//         // Iterate over all slices and copy the contents to the LUE dataset.
//         hsize_t time_idx{0};
//         for(auto const& file_slice_index: slice_indices) {
// 
//             // Determine pathname to GDAL dataset containing bands for
//             // current slice.
//             // Open GDAL dataset.
// 
//             input_dataset_path = stack_slice_path(gdal_dataset_name,
//                 file_slice_index, file_extension);
// 
//             gdal_dataset_ptr = try_open_gdal_raster_dataset_for_read(
//                 input_dataset_path.native());
//             // TODO error handling.
//             assert(gdal_dataset_ptr);
// 
// 
//             // TODO Verify properties of dataset match the one passed
//             // into this function.
// 
//             // TODO Handle all possible value types.
// 
//             // Copy each band to its property value dataset.
//             for(int b = 1; b <= gdal_domain.nr_bands; ++b) {
// 
//                 // Value API:
//                 // - constant collection of shared domain items
//                 // - variable size per item
//                 // - constant size through time
// 
//                 // Write LUE property. -----------------------------------------
//                 // Open property
//                 std::string const property_name = "band_" +
//                     std::to_string(b);
//                 auto& property = property_set.properties().item(property_name);
//                 check_id(lue_dataset_name, property, "open property");
// 
//                 // Open value
//                 auto value = hdf5::Identifier(open_t_n_value(property.id()),
//                     close_t_n_value);
//                 check_id(lue_dataset_name, value, "open value");
// 
// 
//                 // Make sure there is enough space in the dataset of each
//                 // item (1 item in our case).
//                 reserve_t_n_value(value);
// 
// 
//                 // Given a dataset which is large enough, copy the GDAL raster
//                 // band to the corresponding HDF5 dataset.
//                 GDALRasterBand* raster_band =
//                     gdal_dataset_ptr->GetRasterBand(b);
// 
//                 hdf5::Identifier dataset(t_n_value_dataset(value, 0),
//                     ::hdf5_close_dataset);
//                 check_id(lue_dataset_name, dataset, "obtain dataset");
// 
//                 copy_band_t_n(gdal_domain, *raster_band, dataset,
//                     time_idx);
// 
//                 progress_indicator.update_progress(++band_to_copy);
//             }
// 
//             ++time_idx;
//         }
//     }
// 
//     // Write all metadata items in the GDAL dataset to the LUE dataset.
//     // TODO ...
}


// dataset GDAL dataset representing a slice from the stack
// gdal_dataset_name Name of dataset
// start_time_point Time point of first slice
// slice_duration Duration of time step
// lue_dataset_name Name of dataset to write
void translate_gdal_feature_stack_to_lue(
    ::GDALDataset& /* gdal_dataset */,
    std::string const& /* gdal_dataset_name */,
    MonthTimePoint const& /* start_time_point */,
    MonthDuration const& /* slice_duration */,
    std::string const& /* lue_dataset_name */)
{
//     GDALFeatureDomain gdal_domain = gdal_feature_domain(gdal_dataset);
// 
// 
//     // Create LUE dataset.
//     auto lue_dataset = create_dataset(lue_dataset_name);
// 
//     // Add phenomenon to the LUE dataset. Name it after the GDAL dataset.
//     auto const phenomenon_name =
//         boost::filesystem::path(gdal_dataset_name).stem().string();
//     auto& phenomenon = lue_dataset.add_phenomenon(phenomenon_name);
// 
//     // Each feature layer in the GDAL dataset will be mapped to a new
//     // property in a new property set. We assume that, in a GDAL feature
//     // dataset, there are no relations between feature layers.
// 
// 
//     // Time domain contains a single time extent. It is defined by the
//     // begin time point and a duration. This is exactly the same as in the
//     // case of importing a PCRaster temporal stack of fields.
// 
//     // In case the range of slices is continuous (no holes), we only
//     // need to store a single time period. Otherwise we have to treat the
//     // holes as periods with no-data, or store multiple periods. Probably
//     // support no-data periods.
// 
//     // The begin time point is passed in. The duration of the period is
//     // determined by the last slice available.
// 
//     auto const file_extension = gdal_slice_filename_extension(
//         gdal_dataset.GetFileList(), feature_extensions);
//     std::set<size_t> slice_indices = stack_slice_indices(gdal_dataset_name,
//         file_extension);
//     assert(!slice_indices.empty());
// 
//     size_t const first_slice = *slice_indices.begin();
//     size_t const last_slice = *(--slice_indices.end());
//     size_t const nr_slices = last_slice - first_slice + 1;
// 
//     auto const stack_duration = nr_slices * slice_duration;
// 
//     // First slice starts at the time point passed in. It ends after one
//     // slice durations has passed.
//     // {time_point, stack_duration}
// 
// 
//     // Time point types.
//     auto const year_file_type_id = H5T_STD_I32LE;
//     auto const month_file_type_id = H5T_STD_I32LE;
//     auto const year_memory_type_id = H5T_NATIVE_INT32;
//     auto const month_memory_type_id = H5T_NATIVE_INT32;
// 
//     hdf5::Identifier const time_point_file_type(
//         create_month_time_point_file_datatype(year_file_type_id,
//             month_file_type_id), H5Tclose);
//     hdf5::Identifier const time_point_memory_type(
//         create_month_time_point_memory_datatype(year_memory_type_id,
//             month_memory_type_id), H5Tclose);
// 
// 
//     // Time duration types.
//     auto const count_file_type_id = H5T_STD_I32LE;
//     auto const count_memory_type_id = H5T_NATIVE_INT32;
// 
//     hdf5::Identifier const time_duration_file_type(
//         create_month_time_duration_file_datatype(count_file_type_id),
//             H5Tclose);
//     hdf5::Identifier const time_duration_memory_type(
//         create_month_time_duration_memory_datatype(count_memory_type_id),
//             H5Tclose);
// 
// 
//     // Time period types.
//     hdf5::Identifier const time_domain_item_file_type(
//         create_time_period_file_datatype(time_point_file_type,
//             time_duration_file_type), H5Tclose);
//     hdf5::Identifier const time_domain_item_memory_type(
//         create_time_period_memory_datatype(time_point_memory_type,
//             time_duration_memory_type), H5Tclose);
// 
// 
//     hdf5::Identifier space_domain_item_file_type(
//         create_space_point_file_datatype(2, H5T_IEEE_F64LE), H5Tclose);
//     hdf5::Identifier space_domain_item_memory_type(
//         create_space_point_memory_datatype(2, H5T_NATIVE_DOUBLE),
//         H5Tclose);
// 
//     herr_t status;
// 
// 
//     // First layout the LUE dataset, given the GDAL dataset passed in.
// 
//     for(int l = 0; l < gdal_domain.nr_layers; ++l) {
// 
//         OGRLayer& feature_layer{*gdal_dataset.GetLayer(l)};
// 
//         // Add property set to the phenomenon. This one set will contain all
//         // information from the current GDAL feature layer.
//         auto const property_set_name = phenomenon_name +
//             (gdal_domain.nr_layers > 1 ? ("_" + std::to_string(l)) : "");
//         DomainConfiguration domain_configuration(
//             TimeDomainConfiguration{TimeDomainType::shared},
//             // TODO This assumes points. We need to test this.
//             SpaceDomainConfiguration{SpaceDomainItemType::point});
//         auto& property_set = phenomenon.add_property_set(property_set_name,
//             domain_configuration);
// 
//         auto& domain = property_set.domain();
//         hsize_t const nr_items = feature_layer.GetFeatureCount();
// 
//         // domain.attributes().write<std::string>("time_domain_type", "located");
// 
//         // Constant collection of shared domain items.
//         // It is assumed that the collection of property set items
//         // is constant. Before iterating over the feature layers, we
//         // can already write the property set item-id collection.
// 
//         // Item collection.
//         {
//             hdf5::Identifier item_collection(create_item_collection(
//                 domain.id(), LUE_STD_ITEM, nr_items),
//                 close_item_collection);
//             lue_item_t items[nr_items];
//             std::iota(items, items + nr_items, 0);
//             status = write_item_collection(item_collection,
//                 LUE_NATIVE_ITEM, items);
//             check_status(lue_dataset_name, status, "write item collection");
//         }
// 
//         // Time domain.
//         hdf5::Identifier const time_domain(create_shared_time_domain(
//             domain.id(), time_domain_item_file_type),
//             close_shared_time_domain);
// 
//         status = reserve_time_domain_items_space(time_domain, 1);
//         check_status(lue_dataset_name, status,
//             "reserve time domain items space");
// 
// 
//         // TODO Is the layout of TimePeriod guaranteed to be
//         // {
//         //     time_point
//         //     time_duration
//         // }
//         // This is assumed by create_time_period_xxx_datatype().
//         TimePeriod<MonthTimePoint, MonthDuration> time_period(start_time_point,
//             stack_duration);
//         status = write_time_domain_items(time_domain,
//             time_domain_item_memory_type, &time_period, 1);
//         check_status(lue_dataset_name, status, "write time domain items");
// 
// 
//         // Time discretization property.
//         // auto& time_discretization = property_set.add_property(
//         //     "time discretization");
// 
// 
//         // hdf5::Identifier space_domain(hdf5_create_group(domain.id(),
//         //     "space"), hdf5_close_group);
//         // check_id(lue_dataset_name, space_domain, "create space domain");
// 
// 
//         // hdf5::Identifier time_discretization(hdf5_create_group(
//         //     space_domain.id(), "time discretization"), hdf5_close_group);
//         // check_id(lue_dataset_name, time_discretization,
//         //     "create time discretization");
// 
//         // {
//         //     // A 1D array of <rank> values representing the size of
//         //     // the dimensions.
// 
//         //     int const rank = 1;
//         //     hsize_t const shape[rank] = { 1 };
//         //     hsize_t const chunks[rank] = { 1 };
// 
//         //     hdf5::Identifier value(create_value(time_discretization.id(),
//         //         H5T_STD_U64LE, rank, shape, chunks), close_value);
//         //     check_id(lue_dataset_name, value, "create value");
// 
//         //     uint64_t values[1] = { static_cast<uint64_t>(nr_slices) };
// 
//         //     status = write_value(value.id(), H5T_NATIVE_UINT64, values, 1);
//         //     check_status(lue_dataset_name, status, "write value");
//         // }
// 
// 
//         auto& time_discretization = property_set.add_property(
//             "time discretization");
//         check_id(lue_dataset_name, time_discretization, "create property");
// 
//         {
//             // Per item a 1D array of <rank> values representing the size of
//             // the dimensions.
// 
//             int const rank = 1;
//             hsize_t const shape[rank] = { 1 };
//             hsize_t const chunks[rank] = { 1 };
// 
//             hdf5::Identifier value(create_value(time_discretization.id(),
//                 H5T_STD_U64LE, rank, shape, chunks), close_value);
//             check_id(lue_dataset_name, value, "create value");
// 
//             std::vector<uint64_t> values(nr_items, static_cast<uint64_t>(
//                 nr_slices));
//             // uint64_t values[1] = { static_cast<uint64_t>(nr_slices) };
// 
//             herr_t status = write_value(value, H5T_NATIVE_UINT64,
//                 values.data(), nr_items);
//             check_status(lue_dataset_name, status, "write value");
//         }
// 
// 
//         // Space domain.
//         // domain.attributes().write<std::string>("space_domain_type", "located");
// 
//         hdf5::Identifier space_domain(hdf5_create_group(domain.id(),
//             "space"), hdf5_close_group);
//         check_id(lue_dataset_name, space_domain, "create space domain");
// 
//         {
//             int rank = 0;
//             hsize_t const* shape = nullptr;
//             hsize_t const* chunk = nullptr;
// 
//             hdf5::Identifier value(create_t_value(space_domain,
//                 space_domain_item_file_type, rank, shape, chunk,
//                 nr_items), close_t_value);
//         }
// 
//         // Add link between the space domain the time discretization
//         // property.
//         status = hdf5_create_soft_link(time_discretization.id(),
//             "time discretization", space_domain);
//         check_status(lue_dataset_name, status, "link discretization");
// 
// 
// 
//         // Turn all GDAL feature layer fields into properties.
// 
//         auto& feature_definition = *feature_layer.GetLayerDefn();
//         auto const nr_fields = feature_definition.GetFieldCount();
// 
// // std::cout << "name     : " << feature_definition.GetName() << std::endl;
// // std::cout << "nr_fields: " << nr_fields << std::endl;
// 
//         for(int f = 0; f < nr_fields; ++f) {
//             auto& field_definition = *feature_definition.GetFieldDefn(f);
// 
// // std::cout << "    name     : " << field_definition.GetNameRef() << std::endl;
// // std::cout << "    type     : " << field_definition.GetType() << std::endl;
// 
//             auto& property = property_set.add_property(
//                 field_definition.GetNameRef());
//             check_id(lue_dataset_name, property, "create property");
// 
//             hid_t value_type_file_id = -1;
// 
//             switch(field_definition.GetType()) {
//                 case OFTInteger: {
//                     value_type_file_id = H5T_STD_I32LE;
//                     break;
//                 }
//                 case OFTInteger64: {
//                     value_type_file_id = H5T_STD_I64LE;
//                     break;
//                 }
//                 case OFTReal: {
//                     value_type_file_id = H5T_IEEE_F64LE;
//                     break;
//                 }
//                 default: {
//                     // throw std::runtime_error("error writing dataset " +
//                     //     lue_dataset_name + ": unsupported field type");
//                 }
//             }
// 
//             if(value_type_file_id >= 0) {
//                 int rank = 0;
//                 hsize_t const* shape = nullptr;
//                 hsize_t const* chunk = nullptr;
// 
//                 hdf5::Identifier value(create_t_value(property.id(),
//                     value_type_file_id, rank, shape, chunk,
//                     nr_items), close_t_value);
//             }
// 
//             // Add link between the property and the time discretization
//             // property.
//             status = hdf5_create_soft_link(time_discretization.id(),
//                 "time discretization", property.id());
//             check_status(lue_dataset_name, status, "link discretization");
//         }
//     }
// 
// 
//     boost::filesystem::path input_dataset_path;
//     GDALDatasetPtr gdal_dataset_ptr;
//     size_t const nr_layers_to_copy = nr_slices * gdal_domain.nr_layers;
//     size_t layer_to_copy = 0;
// 
// 
//     ProgressIndicator progress_indicator(std::cout, "copying layers",
//         nr_layers_to_copy);
// 
//     // Iterate over all slices and copy the contents to the LUE dataset.
//     hsize_t time_idx{0};
//     for(auto const& file_slice_index: slice_indices) {
// 
//         // Determine pathname to GDAL dataset containing layers for
//         // current slice.
//         // Open GDAL dataset.
// 
//         input_dataset_path = stack_slice_path(gdal_dataset_name,
//             file_slice_index, file_extension);
// 
//         gdal_dataset_ptr = try_open_gdal_feature_dataset_for_read(
//             input_dataset_path.native());
//         // TODO error handling.
//         assert(gdal_dataset_ptr);
// 
// 
//         // TODO Verify properties of dataset match the one passed
//         // into this function.
// 
//         // TODO Handle all possible value types.
// 
//         // Copy each band to its property value dataset.
//         for(int l = 0; l < gdal_domain.nr_layers; ++l) {
// 
//             assert(gdal_dataset_ptr->GetLayer(l));
//             OGRLayer& feature_layer{*(gdal_dataset_ptr->GetLayer(l))};
//             assert(feature_layer.GetGeomType() == wkbPoint);
// 
//             auto const property_set_name = phenomenon_name +
//                 (gdal_domain.nr_layers > 1 ? ("_" + std::to_string(l)) : "");
//             auto& property_set = phenomenon.property_sets().item(
//                 property_set_name);
// 
//             auto& domain = property_set.domain();
//             hdf5::Identifier space_domain(::open_simple_space_domain(
//                 domain.id()), close_simple_space_domain);
// 
//             hsize_t const nr_items = feature_layer.GetFeatureCount();
// 
//             // For the current slice, write the locations to the dataset
//             // with locations per item.
//             // Loop over geometries and obtain coordinates. Assume for now
//             // that we can hold all geometries in memory.
// 
//             auto point_feature = GDALFeaturePtr(
//                 feature_layer.GetNextFeature(), GDALFeatureDeleter{});
//             assert((feature_layer.GetFeatureCount() > 0 && point_feature) ||
//                 !point_feature);
// 
//             std::vector<PointFloat64> points;
//             PointFloat64 a_point;
//             points.reserve(nr_items);
// 
//             // constant collection
//             // shared domain items
//             // constant size per item
//             // constant size through time
// 
//             while(point_feature) {
//                 auto const& point_geometry = *(point_feature->GetGeometryRef());
//                 assert(point_geometry.getDimension() == 0);  // Points
//                 assert(point_geometry.getCoordinateDimension() == 2);  // x, y
// 
//                 // Read all points into memory and write them to the dataset.
//                 auto const& point = dynamic_cast<OGRPoint const&>(
//                     point_geometry);
// 
//                 a_point.x = point.getX();
//                 a_point.y = point.getY();
//                 points.emplace_back(a_point);
// 
//                 point_feature = GDALFeaturePtr(
//                     feature_layer.GetNextFeature(), GDALFeatureDeleter{});
//             }
// 
//             hdf5::Identifier value(open_t_value(space_domain),
//                 close_t_value);
//             check_id(input_dataset_path.native(), value, "open value");
// 
//             status = write_t_value(value,
//                 space_domain_item_memory_type, points.data(),
//                 points.size());
//             check_status(input_dataset_path.native(), status, "write value");
// 
// 
//             auto& feature_definition = *feature_layer.GetLayerDefn();
//             auto const nr_fields = feature_definition.GetFieldCount();
// 
// // std::cout << "name     : " << feature_definition.GetName() << std::endl;
// // std::cout << "nr_fields: " << nr_fields << std::endl;
// 
//             for(int f = 0; f < nr_fields; ++f) {
//                 auto& field_definition = *feature_definition.GetFieldDefn(f);
// 
// // std::cout << "    name     : " << field_definition.GetNameRef() << std::endl;
// // std::cout << "    type     : " << field_definition.GetType() << std::endl;
// 
//                 auto& property = property_set.properties().item(
//                     field_definition.GetNameRef());
//                 check_id(input_dataset_path.native(), property,
//                     "create property");
// 
// 
// 
//                 switch(field_definition.GetType()) {
//                     case OFTInteger: {
//                         std::vector<int32_t> values;
//                         values.reserve(nr_items);
// 
//                         // Read all values.
//                         feature_layer.ResetReading();
//                         auto point_feature = GDALFeaturePtr(
//                             feature_layer.GetNextFeature(),
//                             GDALFeatureDeleter{});
// 
//                         while(point_feature) {
// 
//                             values.emplace_back(
//                                 point_feature->GetFieldAsInteger(f));
// 
//                             point_feature = GDALFeaturePtr(
//                                 feature_layer.GetNextFeature(),
//                                 GDALFeatureDeleter{});
//                         }
// 
// 
//                         // Append all values.
//                         hdf5::Identifier value(open_t_value(property.id()),
//                             close_t_value);
//                         check_id(input_dataset_path.native(), value,
//                             "open value");
// 
//                         status = write_t_value(value,
//                             H5T_NATIVE_INT32, values.data(), values.size());
//                         check_status(input_dataset_path.native(), status,
//                             "write value");
//                         break;
//                     }
//                     case OFTInteger64: {
//                         std::vector<int64_t> values;
//                         values.reserve(nr_items);
// 
//                         // Read all values.
//                         feature_layer.ResetReading();
//                         auto point_feature = GDALFeaturePtr(
//                             feature_layer.GetNextFeature(),
//                             GDALFeatureDeleter{});
// 
//                         while(point_feature) {
// 
//                             values.emplace_back(
//                                 point_feature->GetFieldAsInteger64(f));
// 
//                             point_feature = GDALFeaturePtr(
//                                 feature_layer.GetNextFeature(),
//                                 GDALFeatureDeleter{});
//                         }
// 
// 
//                         // Append all values.
//                         hdf5::Identifier value(open_t_value(property.id()),
//                             close_t_value);
//                         check_id(input_dataset_path.native(), value,
//                             "open value");
// 
//                         status = write_t_value(value,
//                             H5T_NATIVE_INT64, values.data(), values.size());
//                         check_status(input_dataset_path.native(), status,
//                             "write value");
// 
//                         break;
//                     }
//                     case OFTReal: {
//                         std::vector<double> values;
//                         values.reserve(nr_items);
// 
//                         // Read all values.
//                         feature_layer.ResetReading();
//                         auto point_feature = GDALFeaturePtr(
//                             feature_layer.GetNextFeature(),
//                             GDALFeatureDeleter{});
// 
//                         while(point_feature) {
// 
//                             values.emplace_back(
//                                 point_feature->GetFieldAsDouble(f));
// 
//                             point_feature = GDALFeaturePtr(
//                                 feature_layer.GetNextFeature(),
//                                 GDALFeatureDeleter{});
//                         }
// 
// 
//                         // Append all values.
//                         hdf5::Identifier value(open_t_value(property.id()),
//                             close_t_value);
//                         check_id(input_dataset_path.native(), value,
//                             "open value");
// 
//                         status = write_t_value(value,
//                             H5T_NATIVE_DOUBLE, values.data(), values.size());
//                         check_status(input_dataset_path.native(), status,
//                             "write value");
// 
//                         break;
//                     }
//                     default: {
//                         // throw std::runtime_error("error writing dataset " +
//                         //     lue_dataset_name + ": unsupported field type");
//                     }
//                 }
// 
//                 // if(field_definition.GetType() == OFTInteger64) {
//                 //     std::vector<int64_t> values;
//                 //     values.reserve(nr_items);
// 
//                 //     // Read all values.
//                 //     feature_layer.ResetReading();
//                 //     auto point_feature = GDALFeaturePtr(
//                 //         feature_layer.GetNextFeature(), GDALFeatureDeleter{});
// 
//                 //     while(point_feature) {
// 
//                 //         values.emplace_back(
//                 //             point_feature->GetFieldAsInteger64(f));
// 
//                 //         point_feature = GDALFeaturePtr(
//                 //             feature_layer.GetNextFeature(),
//                 //             GDALFeatureDeleter{});
//                 //     }
// 
// 
//                 //     // Append all values.
//                 //     hdf5::Identifier value(open_t_value(property.id()),
//                 //         close_t_value);
//                 //     check_id(input_dataset_path.native(), value, "open value");
// 
//                 //     status = write_t_value(value.id(),
//                 //         H5T_NATIVE_INT64, values.data(), values.size());
//                 //     check_status(input_dataset_path.native(), status,
//                 //         "write value");
//                 // }
//             }
// 
// 
// 
// 
// 
// 
// 
// 
// 
// 
// 
// 
// 
// 
//             progress_indicator.update_progress(++layer_to_copy);
// 
// 
//     ///             // Value API:
//     ///             // - constant collection of shared domain items
//     ///             // - variable size per item
//     ///             // - constant size through time
// 
//     ///             // Write LUE property. -----------------------------------------
//     ///             // Open property
//     ///             std::string const property_name = "band_" +
//     ///                 std::to_string(l);
//     ///             auto& property = property_set.properties().item(property_name);
//     ///             check_id(lue_dataset_name, property, "open property");
// 
//     ///             // Open value
//     ///             auto value = hdf5::Identifier(open_t_n_value(property.id()),
//     ///                 close_t_n_value);
//     ///             check_id(lue_dataset_name, value, "open value");
// 
// 
//     ///             // Make sure there is enough space in the dataset of each
//     ///             // item (1 item in our case).
//     ///             reserve_t_n_value(value.id());
// 
// 
//     ///             // Given a dataset which is large enough, copy the GDAL raster
//     ///             // band to the corresponding HDF5 dataset.
//     ///             GDALRasterBand* raster_band =
//     ///                 gdal_dataset_ptr->GetRasterBand(l);
// 
//     ///             hdf5::Identifier dataset(t_n_value_dataset(value.id(), 0),
//     ///                 ::hdf5_close_dataset);
//     ///             check_id(lue_dataset_name, dataset, "obtain dataset");
// 
//     ///             copy_band_t_n(gdal_domain, *raster_band, dataset.id(),
//     ///                 time_idx);
// 
//     ///             progress_indicator.update_progress(++layer_to_copy);
//     ///         }
// 
//             ++time_idx;
//         }
//     }
// 
//     /// // Write all metadata items in the GDAL dataset to the LUE dataset.
//     /// // TODO ...
}


void translate_gdal_track_to_lue(
    ::GDALDataset& /* gdal_dataset */,
    std::string const& /* gdal_dataset_name */,
    std::string const& lue_dataset_name)
{
    // Read GPS track layers using gdal and write them to a new LUE dataset.
    // Existing information in the target dataset is deleted.

    // GDALRasterDomain gdal_domain = gdal_domain(gdal_dataset);

    // Write information to LUE dataset. ---------------------------------------
    // Open LUE dataset for writing, overwriting existing contents.
    // TODO Add log message to dataset:
    //     <timepoint> Converted using lue_translate <version> using
    //     these arguments: ...
    auto lue_dataset = create_dataset(lue_dataset_name);


    // // Add phenomenon to the LUE dataset. Name it after the GDAL dataset.
    // auto const phenomenon_name =
    //     boost::filesystem::path(gdal_dataset_name).stem().string();
    // auto& phenomenon = lue_dataset.add_phenomenon(phenomenon_name);


    // // Add property set to the phenomenon. This one set will contain all
    // // track layers from the GDAL dataset.
    // auto const property_set_name = "area";
    // auto& property_set = phenomenon.add_property_set(property_set_name);


    // // Add domain to the property set:
    // auto& domain = property_set.domain();
    // assert(domain.id().is_valid());

    // domain.attributes().write<std::string>("time_domain_type", "located");


    // GDALFeatureDomain gdal_domain = gdal_feature_domain(gdal_dataset);

    // hsize_t const nr_items = gdal_domain.nr_bands;
    // herr_t status;

    // A GPX dataset can contain multiple tracks. Here we assume that these
    // tracks are part of a single domain of a single phenomenon item.
    // Multiple items can be supported by loading multiple GPX datasets
    // (in the future).

    // Create time domain.

    // // One file, one item.
    // hsize_t const nr_items = 1;

    // // Use 0 as the id for the one item.
    // std::vector<lue_item_t> item_ids(nr_items);
    // item_ids.push_back(0);

    // // A track is stored as a multi-line string by GDAL. We want to store it
    // // as a sequence of temporal points. One space point per one time point.
    // // The number of time points equals the number of space points (assuming
    // // that no point is recorded at the same time point).
    // // Count the number of points in the track layer.
    // std::vector<hsize_t> nr_domain_items(nr_items);

    // auto const layer = gdal_dataset.GetLayerByName("tracks");
    // assert(layer->GetGeomType() == wkbMultiLineString);
    // assert(layer->GetFeatureCount() == 1);


    // auto const layer_definition = layer->GetLayerDefn();
    // assert(layer_definition);
    // int const nr_fields = layer_definition->GetFieldCount();

    // for(int f = 0; f < nr_fields; ++f) {
    //     auto const field_definition = layer_definition->GetFieldDefn(f);
    //     assert(field_definition);

    //     std::cout << field_definition->GetNameRef() << "\n";
    // }



    // // Use layer name as name of property.
    // auto& property = property_set.add_property(layer->GetName());
    // check_id(lue_dataset_name, property, "create property");

    // auto feature = GDALFeaturePtr(layer->GetNextFeature(),
    //     GDALFeatureDeleter{});

    // feature->DumpReadable(stdout);

    // assert(feature->GetFieldCount() == nr_fields);

    // for(int f = 0; f < nr_fields; ++f) {
    //     std::cout << feature->GetFieldDefnRef(f)->GetNameRef() << ": " << feature->GetFieldAsString(f) << std::endl;

    //     auto const field_list = feature->GetFieldAsStringList(f);
    //     auto const nr_elements = CSLCount(field_list);

    //     for(int e = 0; e < nr_elements; ++e) {
    //         std::cout << "element: " << field_list[e] << std::endl;
    //     }
    // }


    // int const nr_points = feature->GetGeomFieldCount();
    // std::cout << "nr_points: " << nr_points << std::endl;

    // for(int p = 0; p < nr_points; ++p) {
    //     auto const geometry = feature->GetGeomFieldRef(p);

    //     geometry->dumpReadable(stdout);
    // }


    // hier verder
    // tracks laag bevat tracks met multiline string met hoogte (optioneel)
    // track_points laag bevat hoogte (optioneel) en time (optioneel)

    // Loop al deze info af en doe het juist.

    // It also supports reading of route points and track points in standalone
    // layers (route_points and track_points), so that their own attributes
    // can be used by OGR.
    // In addition to its GPX attributes, each route point of a route has
    // a route_fid (foreign key to the FID of its belonging route) and a
    // route_point_id which is its sequence number in the route.
    // The same applies for track points with track_fid, track_seg_id and
    // track_seg_point_id. All coordinates are relative to the WGS84 datum
    // (EPSG:4326).
    // 
    // If the environment variable GPX_ELE_AS_25D is set to YES, the elevation
    // element will be used to set the Z coordinates of waypoints, route points
    // and track points.

    {
    }


    // GPX file contains multiple tracks.
    // A track contains multiple track segments.
    // A track segment contains multiple points.

    // Each track has attributes.
    // Each track segments has attributes.
    // Each point has attributes.

    // This suggests three one normal phenomenon and two relation phenomena:
    // - points → nodes
    // - tracks: track-segment relations → 'multi-arcs'
    // - segments: segment-point relations → arcs


    // For each track:
    // - Add track to track phenomenon, including attributes.
    // - For each track segment:
    //   - Add segment to segment phenomenon, including attributes.
    //   - For each point:
    //     - Add point to point phenomenon, including attributes.
    //   - Relate track segment to begin and end point of segment. Add
    //     relation to segment_point phenomenon.
    // - Relate track to begin and end segment of track. Add relation to
    //   track_segment phenomenon.


    // This relation is special:
    // - Instead of relating one item with one other item, we relate one
    //   item with a group of itemѕ delimited by a first and last item.
    // - The first and last item must come from the same phenomenon.


    // // Setup a phenomenon for the track points.
    // auto const point_phenomenon_name = "point";
    // // auto& point_phenomenon = lue_dataset.add_phenomenon(
    // //     point_phenomenon_name);

    // // Add property set to the phenomenon. This one set will contain all
    // // track points from the GDAL dataset.
    // // auto const point_property_set_name = "point";
    // DomainConfiguration domain_configuration(
    //     TimeDomainConfiguration(TimeDomainType::shared),
    //     SpaceDomainConfiguration(SpaceDomainType::located));
    // // auto& point_property_set = point_phenomenon.add_property_set(
    // //     point_property_set_name, domain_configuration);
    // // auto& point_domain = point_property_set.domain();

    // // point_domain.attributes().write<std::string>("time_domain_type",
    // //     "located");
    // // point_domain.attributes().write<std::string>("space_domain_type",
    // //     "located");


    // // Setup a phenomenon for the track segments.
    // auto const segment_phenomenon_name = "segment";
    // // auto& segment_phenomenon = lue_dataset.add_phenomenon(
    // //     segment_phenomenon_name);

    // // Add property set to the phenomenon. This one set will contain all
    // // track segments from the GDAL dataset.
    // // auto const segment_property_set_name = "segment";
    // domain_configuration = DomainConfiguration(
    //     TimeDomainConfiguration(TimeDomainType::shared),
    //     SpaceDomainConfiguration(SpaceDomainType::located));
    // // auto& segment_property_set = segment_phenomenon.add_property_set(
    // //     segment_property_set_name, domain_configuration);
    // // auto& segment_domain = segment_property_set.domain();

    // // segment_domain.attributes().write<std::string>("time_domain_type",
    // //     "located");
    // // segment_domain.attributes().write<std::string>("space_domain_type",
    // //     "located");


    // // Setup a phenomenon for the tracks.
    // auto const track_phenomenon_name = "track";
    // // auto& track_phenomenon = lue_dataset.add_phenomenon(
    // //     track_phenomenon_name);

    // // Add property set to the phenomenon. This one set will contain all
    // // tracks from the GDAL dataset.
    // auto const track_property_set_name = "track";
    // domain_configuration = DomainConfiguration(
    //     TimeDomainConfiguration(TimeDomainType::shared),
    //     SpaceDomainConfiguration(SpaceDomainType::located));
    // // auto& track_property_set = track_phenomenon.add_property_set(
    // //     track_property_set_name, domain_configuration);

    // // auto& track_domain = track_property_set.domain();

    // // track_domain.attributes().write<std::string>("time_domain_type",
    // //     "located");
    // // track_domain.attributes().write<std::string>("space_domain_type",
    // //     "located");


    // // Read all tracks and write them to the track phenomenon.
    // auto const gdal_track_layer = gdal_dataset.GetLayerByName("tracks");
    // auto const gdal_track_point_layer = gdal_dataset.GetLayerByName(
    //     "track_points");

    // assert(
    //     (gdal_track_layer->GetGeomType() == wkbMultiLineString &&
    //      gdal_track_point_layer->GetGeomType() == wkbPoint) ||
    //     (gdal_track_layer->GetGeomType() == wkbMultiLineString25D &&
    //      gdal_track_point_layer->GetGeomType() == wkbPoint25D));

    // // For now.
    // assert(
    //     (gdal_track_layer->GetGeomType() == wkbMultiLineString25D &&
    //      gdal_track_point_layer->GetGeomType() == wkbPoint25D));

    // // auto const nr_tracks = gdal_track_layer->GetFeatureCount();
    // // auto const nr_points = gdal_track_point_layer->GetFeatureCount();

    // // std::cout << "nr_tracks: " << nr_tracks << std::endl;
    // // std::cout << "nr_points: " << nr_points << std::endl;


    // auto track_feature = GDALFeaturePtr(gdal_track_layer->GetNextFeature(),
    //     GDALFeatureDeleter{});

    // int nr_segments = 0;
    // int nr_points = 0;

    // while(track_feature) {
    //     auto const& track_geometry = *(track_feature->GetGeometryRef());
    //     assert(track_geometry.getDimension() == 1);  // Lines
    //     assert(track_geometry.getCoordinateDimension() == 3);  // x, y, z

    //     auto const& multi_line_string =
    //         dynamic_cast<OGRMultiLineString const&>(track_geometry);

    //     nr_segments += multi_line_string.getNumGeometries();

    //     for(int s = 0; s < multi_line_string.getNumGeometries(); ++s) {
    //         auto const& segment_geometry =
    //             *(multi_line_string.getGeometryRef(s));
    //         assert(segment_geometry.getDimension() == 1);  // Line
    //         assert(segment_geometry.getCoordinateDimension() == 3);  // x, y, z

    //         auto const& line_string = dynamic_cast<OGRLineString const&>(
    //             segment_geometry);

    //         nr_points += line_string.getNumPoints();
    //     }

    //     // TODO Write feature to LUE property set.
    //     // ...

    //     track_feature = GDALFeaturePtr(gdal_track_layer->GetNextFeature(),
    //         GDALFeatureDeleter{});
    // }


    // auto point_feature = GDALFeaturePtr(
    //     gdal_track_point_layer->GetNextFeature(), GDALFeatureDeleter{});

    // while(point_feature) {
    //     auto const& geometry = *(point_feature->GetGeometryRef());
    //     assert(geometry.getDimension() == 0);  // Points
    //     assert(geometry.getCoordinateDimension() == 3);  // x, y, z

    //     auto const& point = dynamic_cast<OGRPoint const&>(geometry);

    //     // std::cout
    //     //     << point.getX() << ", "
    //     //     << point.getY() << ", "
    //     //     << point.getZ() << "\n";

    //     point.dumpReadable(stdout);

    //     // TODO Read all time points in an array.

    //     // TODO Read all space points in an array.




    //     point_feature = GDALFeaturePtr(
    //         gdal_track_point_layer->GetNextFeature(), GDALFeatureDeleter{});
    // }


    // TODO Write all time points to the time domain.

    // TODO Write all space points to the space domain.





    // {
    //     auto const layer_definition = gdal_track_layer->GetLayerDefn();
    //     assert(layer_definition);
    //     int const nr_fields = layer_definition->GetFieldCount();

    //     for(int f = 0; f < nr_fields; ++f) {
    //         auto const field_definition = layer_definition->GetFieldDefn(f);
    //         assert(field_definition);
    //     }

    //     auto feature = GDALFeaturePtr(gdal_track_layer->GetNextFeature(),
    //         GDALFeatureDeleter{});

    //     while(feature) {
    //         feature->DumpReadable(stdout);
    //         feature = GDALFeaturePtr(gdal_track_layer->GetNextFeature(),
    //             GDALFeatureDeleter{});
    //     }
    // }


    // {
    //     auto const layer_definition = gdal_track_point_layer->GetLayerDefn();
    //     assert(layer_definition);
    //     int const nr_fields = layer_definition->GetFieldCount();

    //     for(int f = 0; f < nr_fields; ++f) {
    //         auto const field_definition = layer_definition->GetFieldDefn(f);
    //         assert(field_definition);
    //     }

    //     auto feature = GDALFeaturePtr(gdal_track_point_layer->GetNextFeature(),
    //         GDALFeatureDeleter{});

    //     while(feature) {
    //         feature->DumpReadable(stdout);
    //         feature = GDALFeaturePtr(gdal_track_point_layer->GetNextFeature(),
    //             GDALFeatureDeleter{});
    //     }
    // }









    // Read all points and write them to the point phenomenon.




    // Read all track segments and write them to the track_segment phenomenon.


















    // For each layer:
    // - time domain:
    //     - located in time
    //     - non-shared
    //     - variable collection
    // - space domain:
    //     - 3D space points
    // - for each attribute:
    //     - THERE AREN'T ANY?!













    // Time domain:
    // - Use layer idx as item id.
    // - Store all time points in the time domain.
    //
    // Space domain:
    // - For each time point, store the GPS point in the space domain.
    //
    // Property values:
    // - For each time point, store the attributes as property values.




    // // Item collection.
    // {
    //     hdf5::Identifier item_collection(create_item_collection(
    //         domain.id(), LUE_STD_ITEM, nr_items), close_item_collection);
    //     int const items[nr_items] = { 0 };
    //     status = write_item_collection(item_collection.id(), LUE_NATIVE_ITEM,
    //         items);
    //     check_status(lue_dataset_name, status, "write item collection");
    // }


    // // Space domain.
    // // Domain items are space boxes.
    // {
    //     domain.attributes().write<std::string>("space_domain_type", "located");

    //     int const rank = 2;

    //     hdf5::Identifier space_domain_item_file_type(
    //         create_space_box_file_datatype(rank, H5T_IEEE_F64LE), H5Tclose);
    //     hdf5::Identifier space_domain_item_memory_type(
    //         create_space_box_memory_datatype(rank, H5T_NATIVE_DOUBLE),
    //         H5Tclose);
    //     hdf5::Identifier space_domain(create_space_domain(domain.id(),
    //         space_domain_item_file_type.id(), nr_items), close_space_domain);
    //     // TODO test space_domain.id(), or raise exception

    //     std::vector<BoxFloat64> space_domain_items(nr_items);
    //     space_domain_items[0].min.x = gdal_domain.west;
    //     space_domain_items[0].min.y = gdal_domain.south;
    //     space_domain_items[0].max.x = gdal_domain.east;
    //     space_domain_items[0].max.y = gdal_domain.north;

    //     hdf5::Identifier item_dataset(hdf5_open_dataset(space_domain.id(),
    //         "item"), hdf5_close_dataset);
    //     // TODO test space_domain.id(), or raise exception

    //     status = write_space_domain_items(item_dataset.id(),
    //         space_domain_item_memory_type.id(), space_domain_items.data());
    //     check_status(lue_dataset_name, status, "write space domain items");
    // }


    // // Discretization property.
    // // Domain of discretization is the same as of the band properties so
    // // they can all be part of the same property set. We need to set up
    // // a link between each band property value and the discretization
    // // property.
    // auto& discretization = property_set.add_property("discretization");
    // // TODO test id(), or raise exception
    // check_id(lue_dataset_name, discretization, "create property");

    // {
    //     // Per item a 1D array of <rank> values representing the size of
    //     // the dimensions.

    //     int const rank = 1;
    //     hsize_t const shape[rank] = { 2 };
    //     hsize_t const chunks[rank] = { 2 };

    //     hdf5::Identifier value(create_value(discretization.id(),
    //         H5T_STD_U64LE, rank, shape, chunks), close_value);
    //     check_id(lue_dataset_name, value, "create value");

    //     uint64_t values[2] = { static_cast<uint64_t>(gdal_domain.nr_rows),
    //         static_cast<uint64_t>(gdal_domain.nr_cols) };

    //     herr_t status = write_value(value.id(), H5T_NATIVE_UINT64,
    //         values, nr_items);
    //     check_status(lue_dataset_name, status, "write value");
    // }
}

}  // namespace lue

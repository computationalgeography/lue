/// #pragma once
/// #include "lue/translate/format/gdal.hpp"
/// #include "lue/translate/format/gdal_block.hpp"
/// // #include "lue/utility/progress_indicator.hpp"
/// #include "lue/hdf5/datatype.hpp"
/// #include "lue/translate/format/raster_discretization.hpp"
/// #include "lue/translate/format/raster_domain.hpp"
/// #include <gdal_priv.h>
/// #include <memory>
/// #include <string>
///
///
/// namespace lue {
///     namespace utility {
///
///         GDALDatasetPtr try_open_gdal_raster_dataset_for_read(std::string const& dataset_name);
///
///         GDALDatasetPtr open_gdal_raster_dataset_for_read(std::string const& dataset_name);
///
///         void translate_gdal_raster_dataset_to_lue(
///             std::vector<std::string> const& gdal_dataset_names,
///             std::string const& lue_dataset_name,
///             bool add,
///             Metadata const& metadata);
///
///
///     }  // namespace utility
/// }  // namespace lue

#pragma once
#include <set>
#include <string>
#include <vector>
#include <boost/filesystem.hpp>


namespace lue {

static std::vector<std::string> const raster_extensions = {
    ".map"
};


static std::vector<std::string> const feature_extensions = {
    ".shp"
};


std::string        gdal_slice_filename_extension(
                                        char** pathnames,
                                        std::vector<std::string> const&
                                            supported_filename_extensions);

boost::filesystem::path
                   stack_slice_path    (std::string const& dataset_name,
                                        size_t const index,
                                        std::string const& filename_extension);

std::set<size_t>   stack_slice_indices (std::string const& dataset_name,
                                        std::string const& extension);

std::string        stack_slice_pathname(std::string const& dataset_name,
                                        std::vector<std::string> const&
                                            supported_filename_extensions);

}  // namespace lue

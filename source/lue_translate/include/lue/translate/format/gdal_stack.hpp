#pragma once
#include "lue/translate/format/gdal_raster.hpp"
#include "lue/hl/raster_domain.hpp"
#include "lue/hl/raster_stack_discretization.hpp"
#include <string>


namespace lue {
namespace utility {

GDALDatasetPtr     try_open_gdal_raster_stack_dataset_for_read(
                                        std::string const& dataset_name);

class GDALStack
{

public:

    enum class NamingConvention
    {

        underscore // ,

        // dos_8_3

    };

    static NamingConvention
                   stack_naming_convention(
                                        std::string const& dataset_name);

                   GDALStack           (std::string const& dataset_name);

                   GDALStack           (GDALStack const& other)=default;

                   GDALStack           (GDALStack&& other)=default;

                   ~GDALStack          ()=default;

   GDALStack&      operator=           (GDALStack const& other)=default;

   GDALStack&      operator=           (GDALStack&& other)=default;

   std::string const& dataset_name     () const;

   std::string const& name             () const;

   hl::RasterDomain const&
                   domain              () const;

   hl::RasterStackDiscretization const&
                   discretization      () const;

private:

   std::string     _dataset_name;

   std::string     _name;

   NamingConvention _naming_convention;

   hl::RasterDomain _domain;

   hl::RasterStackDiscretization _discretization;

};

}  // namespace utility
}  // namespace lue

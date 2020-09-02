#pragma once
#include "lue/translate/format/gdal_raster.hpp"
#include "lue/utility/progress_indicator.hpp"
#include "lue/hl/raster_stack.hpp"
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


    using SliceIndex = std::size_t;

    class Slices
    {

    public:

        using Indices = std::vector<SliceIndex>;

        using ConstIterator = Indices::const_iterator;

                   Slices              ()=default;

        explicit   Slices              (Indices indices);

        bool       empty               () const;

        ConstIterator begin            () const;

        ConstIterator end              () const;

        SliceIndex first_index         () const;

        SliceIndex last_index          () const;

        SliceIndex index               (std::size_t step_idx) const;

        std::size_t size               () const;

        std::size_t nr_steps           () const;

        bool       step_available      (std::size_t step_idx) const;

    private:

        Indices _indices;

    };


    class Band
    {

    public:

                   Band                (std::string const& dataset_name,
                                        NamingConvention naming_convention,
                                        Slices const& slices,
                                        int band_nr);

                   Band                (Band const&)=delete;

                   Band                (Band&&)=default;

                   ~Band               ()=default;

        Band&      operator=           (Band const&)=delete;

        Band&      operator=           (Band&&)=default;

        hdf5::Datatype datatype        () const;

        GDALBlock const& slice_blocks  () const;

        std::size_t nr_blocks          () const;

        void       write               (hl::RasterStack::Band& raster_band,
                                        ProgressIndicator& progress_indicator);

    private:

        template<
            typename T>
        void       write               (hl::RasterStack::Band& raster_band,
                                        ProgressIndicator& progress_indicator);

        std::string _dataset_name;

        NamingConvention _naming_convention;

        Slices     _slices;

        int        _band_nr;

        GDALDataType _gdal_datatype;

        GDALBlock  _slice_blocks;

    };


   explicit        GDALStack           (std::string const& dataset_name);

                   GDALStack           (GDALStack const& other)=default;

                   GDALStack           (GDALStack&& other)=default;

                   ~GDALStack          ()=default;

   GDALStack&      operator=           (GDALStack const& other)=default;

   GDALStack&      operator=           (GDALStack&& other)=default;

   std::string const& dataset_name     () const;

   std::string const& name             () const;

   std::size_t     nr_bands            () const;

   hl::RasterDomain const&
                   domain              () const;

   hl::RasterStackDiscretization const&
                   discretization      () const;

   Band            band                (int nr) const;

private:

   std::string     _dataset_name;

   std::string     _name;

   NamingConvention _naming_convention;

   Slices          _slices;

   std::size_t     _nr_bands;

   hl::RasterDomain _domain;

   hl::RasterStackDiscretization _discretization;

};

}  // namespace utility
}  // namespace lue

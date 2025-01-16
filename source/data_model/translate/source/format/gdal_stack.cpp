#include "lue/translate/format/gdal_stack.hpp"
#include <boost/spirit/include/qi.hpp>
#include <filesystem>


namespace lue {
    namespace utility {
        namespace {

            boost::spirit::qi::rule<std::string::iterator, GDALStack::SliceIndex()> stack_rule(
                std::string const& stack_name)
            {
                return stack_name >> boost::spirit::qi::lit("_") >> boost::spirit::qi::uint_ >> ".map";
            }


            namespace underscore {

                gdal::DatasetPtr try_open_gdal_raster_stack_dataset_for_read(std::string const& dataset_name)
                {
                    // What makes a raster a stack:
                    // - dataset_name matches one of the following patterns:
                    //     - <name>_<step>.map
                    //   Only the name part is provided by the user, we need to see whether
                    //   files exists that match the pattern. If they do, pick the first one and
                    //   return the result of calling try_open_gdal_raster_dataset_for_read.
                    //   Otherwise return null.

                    namespace qi = boost::spirit::qi;

                    auto const dataset_path = std::filesystem::path{dataset_name};
                    auto const directory_path = dataset_path.parent_path();
                    gdal::DatasetPtr result;
                    auto stack_rule = utility::stack_rule(dataset_name);

                    // This only works with Boost >= 1.62.0
                    // for(auto const& directory_entry: std::filesystem::directory_iterator(directory_path)) {

                    {
                        std::filesystem::directory_iterator end;

                        for (std::filesystem::directory_iterator it{directory_path}; it != end; ++it)
                        {
                            auto const& directory_entry = *it;
                            auto pathname = directory_entry.path().string();
                            auto first = pathname.begin();
                            auto last = pathname.end();

                            if (qi::parse(first, last, stack_rule))
                            {
                                // Found a file whose name matches the pattern we are
                                // looking for
                                result = try_open_gdal_raster_dataset_for_read(pathname);
                                break;
                            }
                        }
                    }

                    return result;
                }


                bool stack_exists(std::string const& dataset_name)
                {
                    return try_open_gdal_raster_stack_dataset_for_read(dataset_name).operator bool();
                }


                GDALStack::Slices slices(std::string const& dataset_name)
                {
                    namespace qi = boost::spirit::qi;

                    auto stack_rule = utility::stack_rule(dataset_name);
                    auto const dataset_path = std::filesystem::path{dataset_name};
                    auto const directory_path = dataset_path.parent_path();
                    GDALStack::Slices::Indices indices;
                    GDALStack::SliceIndex index;


                    // This only works with Boost >= 1.62.0
                    // for(auto const& directory_entry: std::filesystem::directory_iterator(directory_path)) {

                    {
                        std::filesystem::directory_iterator end;

                        for (std::filesystem::directory_iterator it{directory_path}; it != end; ++it)
                        {
                            auto const& directory_entry = *it;
                            auto pathname = directory_entry.path().string();
                            auto first = pathname.begin();
                            auto last = pathname.end();

                            if (qi::parse(first, last, stack_rule, index))
                            {
                                indices.push_back(index);
                            }
                        }
                    }

                    return GDALStack::Slices{indices};
                }


                std::string slice_dataset_name(
                    std::string const& dataset_name, GDALStack::SliceIndex const slice_idx)
                {
                    return dataset_name + "_" + std::to_string(slice_idx) + ".map";
                }


                gdal::DatasetPtr open_slice_for_read(
                    std::string const& dataset_name, GDALStack::SliceIndex const slice_idx)
                {
                    return open_gdal_raster_dataset_for_read(slice_dataset_name(dataset_name, slice_idx));
                }

            }  // namespace underscore


            gdal::DatasetPtr open_slice_for_read(
                GDALStack::NamingConvention const naming_convention,
                std::string const& dataset_name,
                GDALStack::SliceIndex const slice_idx)
            {
                gdal::DatasetPtr result;

                switch (naming_convention)
                {
                    case GDALStack::NamingConvention::underscore:
                    {
                        result = underscore::open_slice_for_read(dataset_name, slice_idx);
                        break;
                    }
                }

                return result;
            }

        }  // Anonymous namespace


        /*!
            @brief      Try to open a raster dataset read-only
            @return     A pointer to a ::GDALDataset instance if the dataset can be
                        opened. Otherwise a pointer containing nullptr.
        */
        gdal::DatasetPtr try_open_gdal_raster_stack_dataset_for_read(std::string const& dataset_name)
        {
            auto result = underscore::try_open_gdal_raster_stack_dataset_for_read(dataset_name);

            // if(!result) {
            //     result = dos_8_3::try...
            // }

            return result;
        }


        GDALStack::NamingConvention GDALStack::stack_naming_convention(std::string const& dataset_name)
        {
            if (underscore::stack_exists(dataset_name))
            {
                return NamingConvention::underscore;
            }
            // else if(dos_8_3::stack_exists(dataset_name)) {
            //     return NamingConvention::dos_8_3;
            // }
            else
            {
                throw std::runtime_error("Cannot open stack " + dataset_name);
            }
        }


        GDALStack::Slices::Slices(Indices indices):
            _indices{std::forward<Indices>(indices)}
        {
            std::sort(_indices.begin(), _indices.end());
        }


        bool GDALStack::Slices::empty() const
        {
            return _indices.empty();
        }


        GDALStack::Slices::ConstIterator GDALStack::Slices::begin() const
        {
            return _indices.begin();
        }


        GDALStack::Slices::ConstIterator GDALStack::Slices::end() const
        {
            return _indices.end();
        }


        GDALStack::SliceIndex GDALStack::Slices::first_index() const
        {
            assert(!empty());
            return _indices.front();
        }


        GDALStack::SliceIndex GDALStack::Slices::last_index() const
        {
            assert(!empty());
            return _indices.back();
        }


        GDALStack::SliceIndex GDALStack::Slices::index(std::size_t step_idx) const
        {
            return first_index() + step_idx;
        }


        std::size_t GDALStack::Slices::size() const
        {
            return _indices.size();
        }


        std::size_t GDALStack::Slices::nr_steps() const
        {
            // Don't use _indices.size():
            // 1, 2, 3, 4 -> 4 steps
            // 1, 2, 4 -> 4 steps, with one missing!
            return !empty() ? (last_index() - first_index()) + 1 : 0;
        }


        bool GDALStack::Slices::step_available(std::size_t const step_idx) const
        {
            assert(step_idx < nr_steps());

            return std::binary_search(_indices.begin(), _indices.end(), index(step_idx));
        }


        GDALStack::Band::Band(
            std::string const& dataset_name,
            NamingConvention const naming_convention,
            Slices const& slices,
            int const band_nr)

            :
            _dataset_name{dataset_name},
            _naming_convention{naming_convention},
            _slices{slices},
            _band_nr{band_nr},
            _gdal_datatype{},
            _slice_blocks{}

        {
            auto raster =
                GDALRaster{open_slice_for_read(_naming_convention, _dataset_name, _slices.first_index())};
            auto raster_band = raster.band(_band_nr);
            _gdal_datatype = raster_band.gdal_datatype();
            _slice_blocks = raster_band.blocks();
        }


        hdf5::Datatype GDALStack::Band::datatype() const
        {
            return gdal_datatype_to_memory_datatype(_gdal_datatype);
        }


        GDALBlock const& GDALStack::Band::slice_blocks() const
        {
            return _slice_blocks;
        }


        std::size_t GDALStack::Band::nr_blocks() const
        {
            return _slices.size() * _slice_blocks.nr_blocks();
        }


        template<typename T>
        void GDALStack::Band::write(
            hl::RasterStack::Band& raster_stack_band, ProgressIndicator& progress_indicator)
        {
            auto const slice_blocks = this->slice_blocks();
            std::vector<T> values(slice_blocks.block_size());
            size_t nr_valid_cells_x;
            size_t nr_valid_cells_y;
            std::size_t current_block = 0;

            for (std::size_t step_idx = 0; step_idx < _slices.nr_steps(); ++step_idx)
            {

                if (_slices.step_available(step_idx))
                {

                    // Open band from slice corresponding with current time step
                    auto gdal_raster = GDALRaster{
                        open_slice_for_read(_naming_convention, _dataset_name, _slices.index(step_idx))};
                    auto gdal_raster_band = gdal_raster.band(_band_nr);


                    // Copy blocks for current slice to lue dataset
                    for (size_t block_y = 0; block_y < slice_blocks.nr_blocks_y(); ++block_y)
                    {
                        for (size_t block_x = 0; block_x < slice_blocks.nr_blocks_x(); ++block_x)
                        {

                            gdal_raster_band.read_block(block_x, block_y, values.data());

                            std::tie(nr_valid_cells_x, nr_valid_cells_y) =
                                slice_blocks.nr_valid_cells(block_x, block_y);

                            hdf5::Shape const shape = {nr_valid_cells_x * nr_valid_cells_y};
                            auto const memory_dataspace = hdf5::create_dataspace(shape);

                            hdf5::Offset offset = {
                                0,
                                step_idx,
                                block_y * slice_blocks.block_size_y(),
                                block_x * slice_blocks.block_size_x()};
                            hdf5::Count count = {1, 1, nr_valid_cells_y, nr_valid_cells_x};

                            raster_stack_band.write(
                                memory_dataspace, hdf5::Hyperslab{offset, count}, values.data());

                            progress_indicator.update_progress(++current_block);
                        }
                    }
                }
            }
        }


        void GDALStack::Band::write(
            hl::RasterStack::Band& raster_stack_band, ProgressIndicator& progress_indicator)
        {
            switch (_gdal_datatype)
            {
                case GDT_Byte:
                {
                    write<uint8_t>(raster_stack_band, progress_indicator);
                    break;
                }
#if LUE_GDAL_SUPPORTS_8BIT_SIGNED_INTEGERS
                case GDT_Int8:
                {
                    write<int8_t>(raster_stack_band, progress_indicator);
                    break;
                }
#endif
                case GDT_UInt16:
                {
                    write<uint16_t>(raster_stack_band, progress_indicator);
                    break;
                }
                case GDT_Int16:
                {
                    write<int16_t>(raster_stack_band, progress_indicator);
                    break;
                }
                case GDT_UInt32:
                {
                    write<uint32_t>(raster_stack_band, progress_indicator);
                    break;
                }
                case GDT_Int32:
                {
                    write<int32_t>(raster_stack_band, progress_indicator);
                    break;
                }
#if LUE_GDAL_SUPPORTS_64BIT_INTEGERS
                case GDT_UInt64:
                {
                    write<uint64_t>(raster_stack_band, progress_indicator);
                    break;
                }
                case GDT_Int64:
                {
                    write<int64_t>(raster_stack_band, progress_indicator);
                    break;
                }
#endif
                case GDT_Float32:
                {
                    write<float>(raster_stack_band, progress_indicator);
                    break;
                }
                case GDT_Float64:
                {
                    write<double>(raster_stack_band, progress_indicator);
                    break;
                }
                default:
                {
                    throw std::runtime_error("Unsupported datatype");
                }
            }
        }


        GDALStack::GDALStack(std::string const& dataset_name)

            :
            _dataset_name{dataset_name},
            _name{std::filesystem::path(_dataset_name).stem().string()},
            _naming_convention{stack_naming_convention(dataset_name)},
            _slices{},
            _nr_bands{},
            _domain{},
            _discretization{}

        {
            // Open the stack and read the domain and discretization information
            gdal::DatasetPtr raster_dataset;

            switch (_naming_convention)
            {
                case NamingConvention::underscore:
                {
                    _slices = underscore::slices(_dataset_name);
                    raster_dataset = underscore::open_slice_for_read(_dataset_name, _slices.first_index());
                    break;
                }
            }

            assert(!_slices.empty());
            assert(raster_dataset);

            hl::TimeSeriesDiscretization time_series_discretization{_slices.nr_steps()};
            GDALRaster raster{std::move(raster_dataset)};

            _nr_bands = raster.nr_bands();
            _domain = raster.domain();
            _discretization =
                hl::RasterStackDiscretization{time_series_discretization, raster.discretization()};
        }


        std::string const& GDALStack::dataset_name() const
        {
            return _dataset_name;
        }


        std::string const& GDALStack::name() const
        {
            return _name;
        }


        std::size_t GDALStack::nr_bands() const
        {
            return _nr_bands;
        }


        GDALStack::Band GDALStack::band(int nr) const
        {
            assert(nr > 0);
            assert(nr <= int(nr_bands()));

            return Band{_dataset_name, _naming_convention, _slices, nr};
        }


        hl::RasterDomain const& GDALStack::domain() const
        {
            return _domain;
        }


        hl::RasterStackDiscretization const& GDALStack::discretization() const
        {
            return _discretization;
        }

    }  // namespace utility
}  // namespace lue

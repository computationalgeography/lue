#include "lue/translate/format/gdal_stack.hpp"
#include <boost/filesystem.hpp>
#include <boost/spirit/include/qi.hpp>


namespace lue {
namespace utility {
namespace {

using SliceIndex = std::size_t;
using Slice = SliceIndex;

class Slices
{

public:

    Slices()=default;

    Slices(
        std::vector<Slice> slices)
        : _slices{std::forward<std::vector<Slice>>(slices)}
    {
        std::sort(_slices.begin(), _slices.end());
    }

    bool empty() const
    {
        return _slices.empty();
    }

    SliceIndex first_index() const
    {
        assert(!empty());
        return _slices.front();
    }

    SliceIndex last_index() const
    {
        assert(!empty());
        return _slices.back();
    }

    std::size_t nr_steps() const
    {
        // Don't use _slices.size():
        // 1, 2, 3, 4 -> 4 steps
        // 1, 2, 4 -> 4 steps, with one missing!
        return !empty()
            ? (last_index() - first_index()) + 1
            : 0
            ;
    }

private:

    std::vector<Slice> _slices;

};


boost::spirit::qi::rule<std::string::iterator, SliceIndex()> stack_rule(
    std::string const& stack_name)
{
    return
        stack_name >>
        boost::spirit::qi::lit("_") >>
        boost::spirit::qi::uint_ >>
        ".map"
        ;
}


namespace underscore {

GDALDatasetPtr try_open_gdal_raster_stack_dataset_for_read(
    std::string const& dataset_name)
{
    // What makes a raster a stack:
    // - dataset_name matches one of the folowing patterns:
    //     - <name>_<step>.map
    //   Only the name part is provided by the user, we need to see whether
    //   files exists that match the pattern. If they do, pick the first one and
    //   return the result of calling try_open_gdal_raster_dataset_for_read.
    //   Otherwise return null.

    namespace qi = boost::spirit::qi;
    namespace fs = boost::filesystem;

    auto const dataset_path = fs::path(dataset_name);
    auto const directory_path = dataset_path.parent_path();
    GDALDatasetPtr result;

    for(auto const& directory_entry: fs::directory_iterator(directory_path)) {
        auto pathname = directory_entry.path().string();
        auto first = pathname.begin();
        auto last = pathname.end();

        if(qi::parse(first, last, stack_rule(dataset_name))) {
            // Found a file whose name matches the pattern we are
            // looking for
            result = try_open_gdal_raster_dataset_for_read(pathname);
            break;
        }
    }

    return result;
}


bool stack_exists(
    std::string const& dataset_name)
{
    return try_open_gdal_raster_stack_dataset_for_read(dataset_name).
        operator bool();
}


Slices slices(
    std::string const& dataset_name)
{
    namespace qi = boost::spirit::qi;
    namespace fs = boost::filesystem;

    auto const dataset_path = fs::path(dataset_name);
    auto const directory_path = dataset_path.parent_path();
    std::vector<Slice> result;

    SliceIndex index;

    for(auto const& directory_entry: fs::directory_iterator(directory_path)) {
        auto pathname = directory_entry.path().string();
        auto first = pathname.begin();
        auto last = pathname.end();

        if(qi::parse(first, last, stack_rule(dataset_name), index)) {
            result.push_back(index);
        }
    }

    return Slices{result};
}


std::string slice_dataset_name(
    std::string const& dataset_name,
    SliceIndex const slice_idx)
{
    return dataset_name + "_" + std::to_string(slice_idx) + ".map";
}


GDALDatasetPtr open_slice_for_read(
    std::string const& dataset_name,
    SliceIndex const slice_idx)
{
    return open_gdal_raster_dataset_for_read(
        slice_dataset_name(dataset_name, slice_idx));
}

}  // namespace underscore
}  // Anonymous namespace


/*!
    @brief      Try to open a raster dataset read-only
    @return     A pointer to a ::GDALDataset instance if the dataset can be
                opened. Otherwise a pointer containing nullptr.
*/
GDALDatasetPtr try_open_gdal_raster_stack_dataset_for_read(
    std::string const& dataset_name)
{
    auto result = underscore::try_open_gdal_raster_stack_dataset_for_read(
        dataset_name);

    // if(!result) {
    //     result = dos_8_3::try...
    // }

    return result;
}


GDALStack::NamingConvention GDALStack::stack_naming_convention(
    std::string const& dataset_name)
{
    if(underscore::stack_exists(dataset_name)) {
        return NamingConvention::underscore;
    }
    // else if(dos_8_3::stack_exists(dataset_name)) {
    //     return NamingConvention::dos_8_3;
    // }
    else {
        throw std::runtime_error("Cannot open stack " + dataset_name);
    }
}


GDALStack::GDALStack(
    std::string const& dataset_name)

    : _dataset_name{dataset_name},
      _name{boost::filesystem::path(_dataset_name).stem().string()},
      _naming_convention{stack_naming_convention(dataset_name)},
      _domain{},
      _discretization{}

{
    // Open the stack and read the domain and discretization information
    Slices slices;
    GDALDatasetPtr raster_dataset;

    switch(_naming_convention) {
        case NamingConvention::underscore: {
            slices = underscore::slices(_dataset_name);
            raster_dataset = underscore::open_slice_for_read(
                _dataset_name, slices.first_index());
            break;
        }
    }

    assert(!slices.empty());
    assert(raster_dataset);

    hl::TimeSeriesDiscretization time_series_discretization{slices.nr_steps()};
    GDALRaster raster{std::move(raster_dataset)};

    if(raster.nr_bands() > 1) {
        throw std::runtime_error(
            "GDAL raster stacks with more than 1 band are not supported yet");
    }

    _domain = raster.domain();
    _discretization = hl::RasterStackDiscretization{
        time_series_discretization,
        raster.discretization()};
}


std::string const& GDALStack::dataset_name() const
{
    return _dataset_name;
}


std::string const& GDALStack::name() const
{
    return _name;
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

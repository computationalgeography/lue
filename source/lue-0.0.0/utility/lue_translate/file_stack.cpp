// #define BOOST_SPIRIT_X3_DEBUG
#include "lue/utility/lue_translate/file_stack.h"
#include <cassert>
#include <iostream>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/spirit/home/x3.hpp>


namespace lue {

std::string gdal_slice_filename_extension(
    char** pathnames,
    std::vector<std::string> const& supported_filename_extensions)
{
    std::string extension;
    std::string an_extension;

    for(char const* pathname = pathnames[0]; extension.empty() &&
            pathname != NULL; ++pathname) {
        auto an_extension =
            boost::filesystem::path(pathname).extension().string();

        if(std::find(supported_filename_extensions.begin(),
                supported_filename_extensions.end(), an_extension) !=
                    supported_filename_extensions.end()) {
            extension = an_extension;
        }
    }

    assert(extension.empty() || extension[0] == '.');

    return extension;
}


boost::filesystem::path stack_slice_path(
    std::string const& dataset_name,
    size_t const index,
    std::string const& filename_extension)
{
    std::string const pathname = boost::str(boost::format("%1%_%2%%3%")
        % dataset_name
        % index
        % filename_extension);

    boost::filesystem::path const input_dataset_path(pathname);

    return input_dataset_path;
}


std::set<size_t> stack_slice_indices(
    std::string const& dataset_name,
    std::string const& extension)
{
    // Get a list of filenames.
    boost::filesystem::path const input_dataset_path(dataset_name);
    // TODO Call lexically_normal(), which requires boost >= 1.60 (or C++17?).

    auto directory_iterator = boost::filesystem::directory_iterator(
        input_dataset_path.parent_path());
    auto const directory_iterator_end = boost::filesystem::directory_iterator();

    // <basename> -> <basename>_<step>.map
    static auto const stack_rule1 =
        boost::spirit::x3::rule<class stack_rule1, size_t>() =
            boost::spirit::x3::lit(dataset_name.c_str()) >>
            '_' >>
            boost::spirit::x3::uint_ >>
            boost::spirit::x3::lit(extension.c_str())
            ;
    size_t index;
    std::set<size_t> indices;
    std::string pathname;

    for(; directory_iterator != directory_iterator_end; ++directory_iterator) {

        pathname = directory_iterator->path().string();

        auto first = pathname.begin();
        auto last = pathname.end();

        // if(boost::spirit::x3::parse(first, last, stack_rule1)) {
        if(boost::spirit::x3::phrase_parse(first, last, stack_rule1,
               boost::spirit::x3::space, index)) {
            assert(first == last);
            indices.insert(index);
        }
    }

    return indices;
}


// It is not guaranteed that the pathname returned points to a GDAL dataset
// (can be opened by GDAL). This function just scans filenames.
std::string stack_slice_pathname(
    std::string const& dataset_name,
    std::vector<std::string> const& supported_filename_extensions)
{
    // Get a list of filenames.
    boost::filesystem::path const input_dataset_path(dataset_name);
    // TODO Call lexically_normal(), which requires boost >= 1.60 (or C++17?).

    auto directory_iterator = boost::filesystem::directory_iterator(
        input_dataset_path.parent_path());
    auto const directory_iterator_end =
        boost::filesystem::directory_iterator();

    bool stack_found = false;
    std::string pathname;


    for(std::string const& extension: supported_filename_extensions) {

        // <basename> -> <basename>_<step>.<extension>
        auto stack_rule1 =
            boost::spirit::x3::lit(dataset_name.c_str()) >>
            '_' >>
            boost::spirit::x3::uint_ >>
            boost::spirit::x3::lit(extension.c_str())
            ;

        for(; !stack_found && directory_iterator != directory_iterator_end;
                ++directory_iterator) {

            pathname = directory_iterator->path().string();

            auto first = pathname.begin();
            auto last = pathname.end();

            if(boost::spirit::x3::parse(first, last, stack_rule1)) {
                assert(first == last);
                stack_found = true;
            }
        }

        if(stack_found) {
            break;
        }
    }


    return stack_found ? pathname : "";
}

}  // namespace lue

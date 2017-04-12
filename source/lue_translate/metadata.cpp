#include "lue_translate/metadata.h"
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <exception>


namespace lue {
namespace utility {

/*!
    @brief      Construct an instance given a pathname to a file containing
                metadata items
    @exception  std::runtime_error In case the file does not exist
*/
Metadata::Metadata(
    std::string const& pathname)

    : _json()

{
    namespace bfs = boost::filesystem;

    if(!bfs::exists(pathname)) {
        throw std::runtime_error(boost::str(boost::format(
            "File containing metadata (%1%) does not exist") % pathname));
    }

    std::ifstream(pathname) >> _json;
}


/*!
    @brief      Construct an instance given an input stream containing
                metadata items
*/
Metadata::Metadata(
    std::istream& stream)

    : _json()

{
    stream >> _json;
}


/*!
    @brief      Return the string value pointed to by @a path, or @a
                default_value in case no such value exists at that
                location
*/
std::string Metadata::value(
    std::string const& path,
    std::string const& default_value) const
{
    return value<std::string>(path, default_value);
}

}  // namespace utility
}  // namespace lue

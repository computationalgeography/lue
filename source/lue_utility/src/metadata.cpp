#include "lue/utility/metadata.hpp"
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <exception>


namespace lue {
namespace utility {
namespace json {

bool has_key(
    JSON const& object,
    std::string const& name)
{
    return !object.is_null() && object.find(name) != object.end();
}


bool has_key(
    JSON const& object,
    JSONPointer const& pointer)
{
    return !object.is_null() && ! object.value(pointer, JSON()).is_null();
}


void assert_has_key(
    JSON const& object,
    std::string const& name)
{
    if(!has_key(object, name)) {
        throw std::runtime_error(boost::str(boost::format(
            "JSON object does not contain key %1%") % name));
    }
}


void assert_has_key(
    JSON const& object,
    JSONPointer const& pointer)
{
    if(!has_key(object, pointer)) {
        throw std::runtime_error(boost::str(boost::format(
            "JSON object does not contain key %1%")
                % std::string(pointer)));
    }
}


JSON object(
    JSON const& object,
    std::string const& name)
{
    assert_has_key(object, name);

    return object[name];
}


JSON object(
    JSON const& object,
    JSONPointer const& pointer)
{
    assert_has_key(object, pointer);

    return object[pointer];
}


std::string string(
    JSON const& object,
    std::string const& name)
{
    return json::object(object, name);
}


std::string string(
    JSON const& object,
    JSONPointer const& pointer)
{
    return json::object(object, pointer);
}


bool boolean(
    JSON const& object,
    JSONPointer const& pointer)
{
    return json::object(object, pointer);
}


JSONPointer pointer(
    JSON const& object,
    std::string const& name)
{
    return JSONPointer(string(object, name));
}


JSONCIterator find(
    JSON const& object,
    std::string const& name,
    std::string const& string)
{
    JSONCIterator it;

    for(it = object.begin(); it != object.end(); ++it) {
        // The key must exist.
        assert_has_key(*it, name);

        if(json::string((*it), name) == string) {
            break;
        }
    }

    return it;
}

}  // namespace json


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


JSON const& Metadata::object() const
{
    return _json;
}


std::string Metadata::string(
    JSONPointer const& pointer,
    std::string const& default_value) const
{
    return json::has_key(_json, pointer)
        ? json::string(_json, pointer)
        : default_value;
}


bool Metadata::boolean(
    JSONPointer const& pointer,
    bool const default_value) const
{
    return json::has_key(_json, pointer)
        ? json::boolean(_json, pointer)
        : default_value;
}


std::string Metadata::string(
    std::string const& list_name,
    std::string const& key,
    std::string const& value,
    JSONPointer const& pointer,
    std::string const& default_value) const
{
    std::string result = default_value;

    // Check whether a json element with the name of the list to search
    // exists.
    if(json::has_key(_json, list_name)) {
        // Obtain the object that should be the list.
        auto const list_json = json::object(_json, list_name);

        // Find the object that contains an element with a (key, value).
        auto const object_json_it = json::find(list_json, key, value);

        if(object_json_it != list_json.end()) {
            auto const& object_json = *object_json_it;

            // Find the string pointed to by the pointer.
            if(json::has_key(object_json, pointer)) {
                result = json::string(object_json, pointer);
            }
        }
    }

    return result;
}


std::string Metadata::string(
    std::string const& list_name,
    std::string const& key,
    std::string const& value,
    JSONPointer const& pointer,
    std::string const& value_key,
    std::string const& default_value) const
{
    std::string result = default_value;

    // Check whether a json element with the name of the list to search
    // exists.
    if(json::has_key(_json, list_name)) {
        // Obtain the object that should be the list.
        auto const list_json = json::object(_json, list_name);

        // Find the object that contains an element with a (key, value).
        auto const object_json_it = json::find(list_json, key, value);

        if(object_json_it != list_json.end()) {
            auto const& object_json = *object_json_it;

            // Find the string pointed to by the pointer.
            if(json::has_key(object_json, pointer)) {

                auto const& object2_json = json::object(object_json, pointer);

                if(json::has_key(object2_json, value_key)) {
                    result = json::string(object2_json, value_key);
                }
            }
        }
    }

    return result;
}

}  // namespace utility
}  // namespace lue

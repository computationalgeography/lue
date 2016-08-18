#include "lue/cxx_api/collection.h"
#include "lue/c_api/collection.h"


namespace lue {

bool collection_exists(
    hdf5::Identifier const& location,
    std::string const& name)
{
    return ::collection_exists(location, name.c_str()) > 0;
}


std::vector<std::string> item_names(
    hdf5::Identifier const& location)
{
    size_t const nr_items = ::nr_items(location);

    // Allocate space for pointers to characters. Ask for the names.
    std::vector<char*> c_names(nr_items);
    ::item_names(location, c_names.data());

    // Copy the names to string instances. Free the allocated memory.
    std::vector<std::string> names(nr_items);

    for(size_t i = 0; i < names.size(); ++i) {
        names[i] = c_names[i];
        free(c_names[i]);
    }

    return names;
}


/*!
    @ingroup    lue_cxx_api_group
*/
hdf5::Identifier create_collection(
    hdf5::Identifier const& location,
    std::string const& name)
{
    assert(location.is_valid());

    if(collection_exists(location, name)) {
        throw std::runtime_error("Collection already exists");
    }

    hdf5::Identifier collection_location(::create_collection(location,
        name.c_str()), ::close_collection);

    if(!collection_location.is_valid()) {
        throw std::runtime_error("Collection cannot be created");
    }

    return collection_location;
}

} // namespace lue

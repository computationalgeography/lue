#include "lue/cxx_api/property_set_configuration.h"
#include "lue/cxx_api/enum_string_bimap.h"


namespace lue {
namespace {

detail::EnumStringBimap<SizeOfItemCollectionType> const
        size_of_item_collection_type_map = {
    { SizeOfItemCollectionType::constant_size, "constant_size" },
    // { SizeOfItemCollectionType::variable_size, "variable_size" }
};

}  // Anonymous namespace


/*!
    @ingroup    lue_cxx_api_group
*/
std::string size_of_item_collection_type_to_string(
    SizeOfItemCollectionType const type)
{
    return size_of_item_collection_type_map.as_string(type);
}


/*!
    @ingroup    lue_cxx_api_group
*/
SizeOfItemCollectionType parse_size_of_item_collection_type(
    std::string const& string)
{
    if(!size_of_item_collection_type_map.contains(string)) {
        throw std::runtime_error("Unknown size of item collection type: " +
            string);
    }

    return size_of_item_collection_type_map.as_value(string);
}


PropertySetConfiguration::PropertySetConfiguration(
    SizeOfItemCollectionType const type)

    : _size_of_item_collection_type{type}

{
}


SizeOfItemCollectionType
    PropertySetConfiguration::size_of_item_collection_type() const
{
    return _size_of_item_collection_type;
}

}  // namespace lue

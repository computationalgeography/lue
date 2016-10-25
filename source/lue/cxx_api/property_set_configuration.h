#pragma once
#include "lue/cxx_api/define.h"
#include <string>


namespace lue {

std::string        size_of_item_collection_type_to_string(
                                        SizeOfItemCollectionType const type);

SizeOfItemCollectionType
                   parse_size_of_item_collection_type(
                                        std::string const& string);


/*!
    @ingroup    lue_cxx_api_group
*/
class PropertySetConfiguration
{

public:

                   PropertySetConfiguration(
                                       SizeOfItemCollectionType const type);

                   ~PropertySetConfiguration()=default;

    SizeOfItemCollectionType
                   size_of_item_collection_type() const;

private:

    SizeOfItemCollectionType
                   _size_of_item_collection_type;

};

}  // namespace lue

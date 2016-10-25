#pragma once
#include "lue/cxx_api/define.h"
#include <string>


namespace lue {

std::string        shape_per_item_type_to_string(
                                        ShapePerItemType const type);

ShapePerItemType   parse_shape_per_item_type(
                                        std::string const& string);


/*!
    @ingroup    lue_cxx_api_group
*/
class ValueConfiguration
{

public:

                   ValueConfiguration  (ShapePerItemType const type);

                   ~ValueConfiguration ()=default;

    ShapePerItemType
                   shape_per_item_type () const;

private:

    ShapePerItemType
                   _shape_per_item_type;

};

}  // namespace lue

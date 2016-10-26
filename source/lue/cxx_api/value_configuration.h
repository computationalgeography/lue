#pragma once
#include "lue/cxx_api/define.h"
#include <string>


namespace lue {

std::string        shape_through_time_to_string(
                                        ShapeThroughTime const
                                            shape_through_time);

ShapeThroughTime   parse_shape_through_time(
                                        std::string const& string);

std::string        shape_per_item_to_string(
                                        ShapePerItem const shape_per_item);

ShapePerItem       parse_shape_per_item(std::string const& string);


/*!
    @ingroup    lue_cxx_api_group
*/
class ValueConfiguration
{

public:

                   ValueConfiguration  (ShapeThroughTime const
                                            shape_through_time,
                                        ShapePerItem const shape_per_item);

                   ~ValueConfiguration ()=default;

    ShapeThroughTime
                   shape_through_time  () const;

    ShapePerItem   shape_per_item      () const;

private:

    ShapeThroughTime
                   _shape_through_time;

    ShapePerItem   _shape_per_item;

};

}  // namespace lue

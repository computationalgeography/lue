#pragma once
#include "lue/framework/core/assert.hpp"
#include <cmath>


namespace lue {

// template<
//     typename Input,
//     typename Output>
// double slope(
//     Input const from_range,
//     Output const to_range)
// {
//     lue_hpx_assert(from_range > 0);
//     lue_hpx_assert(to_range >= 0);
// 
//     return static_cast<double>(to_range) / (from_range);
// }
// 
// 
// template<
//     typename Input,
//     typename Output>
// double slope(
//     Input const from_min,
//     Input const from_max,
//     Output const to_min,
//     Output const to_max)
// {
//     return slope(from_max - from_min, to_max - to_min);
// }
// 
// 
// /*!
//     @brief      Map a value from an input range to an output range
// */
// template<
//     typename Input,
//     typename Output>
// Output map_to_range(
//     Input const from_min,
//     Output const to_min,
//     double const slope,
//     Input const from_value)
// {
//     // This code possibly only handles the cases from the unit tests
//     lue_hpx_assert(from_value >= from_min);
// 
//     Output const to_value =
//         static_cast<Output>(
//             to_min + std::round(slope * (from_value - from_min)));
// 
//     lue_hpx_assert(to_value >= to_min);
// 
//     return to_value;
// }
// 
// 
// /*!
//     @brief      Map a value from an input range to an output range
// */
// template<
//     typename Input,
//     typename Output>
// Output map_to_range(
//     Input const from_min,
//     Input const from_max,
//     Output const to_min,
//     Output const to_max,
//     Input const from_value)
// {
//     // This code possibly only handles the cases from the unit tests
//     lue_hpx_assert(from_value >= from_min);
//     lue_hpx_assert(from_value <= from_max);
// 
//     Output to_value = to_min;
// 
//     if(from_min < from_max) {
//         double const slope = lue::slope(from_min, from_max, to_min, to_max);
//         to_value = map_to_range(from_min, to_min, slope, from_value);
//     }
// 
//     lue_hpx_assert(to_value >= to_min);
//     lue_hpx_assert(to_value <= to_max);
// 
//     return to_value;
// }


/*!
    @brief      Map a value from an input range to an output range
*/
template<
    typename Input,
    typename Output>
Output map_to_range(
    Input const from_min,
    Input const from_max,
    Output const to_min,
    Output const to_max,
    Input const from_value)
{
    // This code possibly only handles the cases from the unit tests
    lue_hpx_assert(from_min <= from_max);
    lue_hpx_assert(to_min <= to_max);
    lue_hpx_assert(from_value >= from_min);
    lue_hpx_assert(from_value <= from_max);

    Output result = to_min;

    if(from_min < from_max) {

        double const input_range = (from_max - from_min) + 1;
        lue_hpx_assert(input_range >= 0);

        double const output_range = (to_max - to_min) + 1;
        lue_hpx_assert(output_range >= 0);

        double const factor = output_range / input_range;

        result += std::floor(factor * (from_value - from_min));

    }

    lue_hpx_assert(result >= to_min);
    lue_hpx_assert(result <= to_max);

    return result;
}

}  // namespace lue

#pragma once
#include "lue/framework/core/define.hpp"


namespace lue {
namespace policy {

/*!
    @brief      Input no-data policy that does nothing

    Use this policy if you do not want to check for no-data values in
    the input.
*/
class SkipNoData
{

    public:

        static constexpr bool is_no_data()
        {
            return false;
        }

        static constexpr bool is_no_data(
            Index /* idx */ ...)
        {
            return false;
        }

};

}  // namespace policy
}  // namespace lue

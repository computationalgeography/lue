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

        template<
            typename Data>
        static constexpr bool is_no_data(
            Data const& /* data */)
        {
            return false;
        }

        template<
            typename Data>
        static constexpr bool is_no_data(
            Data const& /* data */,
            Index /* idx */ ...)
        {
            return false;
        }

};

}  // namespace policy
}  // namespace lue

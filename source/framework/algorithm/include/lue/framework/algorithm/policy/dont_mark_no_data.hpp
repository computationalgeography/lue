#pragma once
#include "lue/framework/algorithm/policy/skip_no_data.hpp"
#include "lue/framework/core/define.hpp"


namespace lue {
namespace policy {

/*!
    @brief      Output no-data policy that does nothing

    Use this policy if you do not want to mark no-data values in
    the output.
*/
class DontMarkNoData:
    public SkipNoData
{

    public:

        template<
            typename Data>
        static constexpr void mark_no_data(
            Data& /* data */)
        {
        }

        template<
            typename Data>
        static constexpr void mark_no_data(
            Data& /* data */,
            Index /* idx */ ...)
        {
        }

};

}  // namespace policy
}  // namespace lue

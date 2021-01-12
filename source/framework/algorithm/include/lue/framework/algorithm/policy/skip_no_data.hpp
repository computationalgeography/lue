#pragma once
#include "lue/framework/core/define.hpp"
#include <hpx/serialization.hpp>


namespace lue {
namespace policy {

/*!
    @brief      Input no-data policy that does nothing

    Use this policy if you do not want to check for no-data values in
    the input.
*/
template<
    typename Element>
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

    private:

        friend class hpx::serialization::access;

        template<typename Archive>
        void serialize(
            [[maybe_unused]] Archive& archive,
            [[maybe_unused]] unsigned int const version)
        {
            // Nothing to do
        }

};

}  // namespace policy
}  // namespace lue

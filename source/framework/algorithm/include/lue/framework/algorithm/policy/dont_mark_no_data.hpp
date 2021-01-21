#pragma once
#include "lue/framework/algorithm/policy/skip_no_data.hpp"


namespace lue {
namespace policy {

/*!
    @brief      Output no-data policy that does nothing

    Use this policy if you do not want to mark no-data values in
    the output.
*/
template<
    typename Element>
class DontMarkNoData:
    public SkipNoData<Element>
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


namespace detail {

template<
    typename E>
class TypeTraits<
    DontMarkNoData<E>>
{

    public:

        using Element = E;

        template<
            typename E_>
        using Policy = DontMarkNoData<E_>;

};

}  // namespace detail
}  // namespace policy
}  // namespace lue

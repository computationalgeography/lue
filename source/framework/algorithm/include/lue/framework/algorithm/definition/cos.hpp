#pragma once
#include "lue/framework/algorithm/cos.hpp"
#include "lue/framework/algorithm/definition/unary_local_operation.hpp"
#include "lue/macro.hpp"
#include <cmath>


namespace lue {
    namespace detail {

        template<
            typename InputElement>
        class Cos
        {

            public:

                static_assert(std::is_floating_point_v<InputElement>);

                using OutputElement = InputElement;


                OutputElement operator()(
                    InputElement const& input_element) const noexcept
                {
                    return std::cos(input_element);
                }

        };

    }  // namespace detail


    template<
        typename Element,
        Rank rank>
    PartitionedArray<Element, rank> cos(
        PartitionedArray<Element, rank> const& array)
    {
        using Functor = detail::Cos<Element>;
        using Policies = policy::cos::DefaultPolicies<Element>;

        return unary_local_operation(Policies{}, array, Functor{});
    }

}  // namespace lue

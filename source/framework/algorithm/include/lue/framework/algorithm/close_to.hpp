#pragma once
#include "lue/framework/algorithm/binary_local_operation.hpp"
#include "lue/framework/algorithm/policy.hpp"
#include <hpx/serialization/serialize.hpp>
#include <cmath>


namespace lue {
    namespace detail {

        // https://docs.scipy.org/doc/numpy/reference/generated/numpy.isclose.html
        template<typename InputElement, typename OutputElement_>
        class CloseTo
        {

            public:

                static_assert(std::is_floating_point_v<InputElement>);
                static_assert(std::is_integral_v<OutputElement_>);

                static constexpr char const* name{"close_to"};

                using OutputElement = OutputElement_;


                CloseTo(
                    InputElement const relative_difference = 1e-5,
                    InputElement const absolute_difference = 1e-8):

                    _relative_difference{relative_difference},
                    _absolute_difference{absolute_difference}

                {
                }


                constexpr auto operator()(
                    InputElement const& input_element1, InputElement const& input_element2) const noexcept
                    -> OutputElement
                {
                    return std::abs(input_element1 - input_element2) <=
                           (_absolute_difference + _relative_difference * std::abs(input_element2));
                }


                friend class hpx::serialization::access;


                template<typename Archive>
                void serialize(Archive& archive, unsigned int const /* version */)
                {
                    // clang-format off
                    archive
                        & _relative_difference
                        & _absolute_difference
                        ;
                    // clang-format on
                }


            private:

                InputElement _relative_difference;

                InputElement _absolute_difference;
        };

    }  // namespace detail


    LUE_BINARY_LOCAL_OPERATION_OVERLOADS_WITH_POLICIES_DIFFERENT_OUTPUT_ELEMENT(close_to, detail::CloseTo)

}  // namespace lue

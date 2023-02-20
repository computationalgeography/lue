#pragma once
#include "lue/framework/algorithm/policy/policy_traits.hpp"
#include <hpx/serialization.hpp>


namespace lue::policy {

    template<typename OutputNoDataPolicy, typename RangePolicy>
    class OutputPolicies
    {

        public:

            OutputPolicies():

                _output_no_data_policy{},
                _range_policy{}

            {
            }


            OutputPolicies(OutputNoDataPolicy const& ondp, RangePolicy const& rp = RangePolicy{}):

                _output_no_data_policy{ondp},
                _range_policy{rp}

            {
            }


            OutputNoDataPolicy const& output_no_data_policy() const
            {
                return _output_no_data_policy;
            }


            RangePolicy const& range_policy() const
            {
                return _range_policy;
            }


        private:

            friend class hpx::serialization::access;


            template<typename Archive>
            void serialize(Archive& archive, [[maybe_unused]] unsigned int const version)
            {
                archive& _output_no_data_policy& _range_policy;
            }


            OutputNoDataPolicy _output_no_data_policy;

            RangePolicy _range_policy;
    };


    namespace detail {

        template<typename OutputNoDataPolicy_, typename RangePolicy_>
        class TypeTraits<OutputPolicies<OutputNoDataPolicy_, RangePolicy_>>
        {

            public:

                using OutputNoDataPolicy = OutputNoDataPolicy_;

                using RangePolicy = RangePolicy_;

                using Element = ElementT<OutputNoDataPolicy>;

                // TODO Passing in the original Range policy. Not sure how to create the correct
                //      type for a range policy based on only the output element type. Not
                //      sure that is needed at all. At the call site, we probably only need
                //      the output no-data policy. If so, obtain only that type.
                template<typename Element>
                using Policies =
                    OutputPolicies<OutputNoDataPolicyT<OutputNoDataPolicy, Element>, RangePolicy>;

                using InputNoDataPolicy = typename TypeTraits<OutputNoDataPolicy>::InputNoDataPolicy;
        };

    }  // namespace detail
}  // namespace lue::policy

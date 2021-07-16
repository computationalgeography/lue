#pragma once
#include "lue/framework/algorithm/policy/policy_traits.hpp"
#include <hpx/serialization.hpp>


namespace lue::policy {

    template<
        typename OutputNoDataPolicy>
    class OutputPolicies
    {

        public:

            OutputPolicies():

                _ondp{}

            {
            }


            OutputPolicies(
                OutputNoDataPolicy const& ondp):

                _ondp{ondp}

            {
            }


            OutputNoDataPolicy const& output_no_data_policy() const
            {
                return _ondp;
            }


        private:

            friend class hpx::serialization::access;


            template<typename Archive>
            void serialize(
                Archive& archive,
                [[maybe_unused]] unsigned int const version)
            {
                archive & _ondp;
            }


            OutputNoDataPolicy _ondp;

    };


    namespace detail {

        template<
            typename OutputNoDataPolicy_>
        class TypeTraits<
            OutputPolicies<OutputNoDataPolicy_>>
        {

            public:

                using OutputNoDataPolicy = OutputNoDataPolicy_;

                using Element = ElementT<OutputNoDataPolicy>;

                template<
                    typename Element>
                using Policies = OutputPolicies<
                    OutputNoDataPolicyT<OutputNoDataPolicy, Element>>;

                using InputNoDataPolicy = typename TypeTraits<OutputNoDataPolicy>::InputNoDataPolicy;

        };

    }  // namespace detail
}  // namespace lue::policy

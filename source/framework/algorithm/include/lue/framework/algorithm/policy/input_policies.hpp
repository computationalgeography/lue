#pragma once
#include "lue/framework/algorithm/policy/policy_traits.hpp"
#include <hpx/serialization.hpp>


namespace lue::policy {

    template<typename InputNoDataPolicy>
    class InputPolicies
    {

        public:

            InputPolicies():

                _indp{}

            {
            }

            InputPolicies(InputNoDataPolicy const& indp):

                _indp{indp}

            {
            }

            virtual ~InputPolicies() = default;

            InputNoDataPolicy const& input_no_data_policy() const
            {
                return _indp;
            }

        protected:

            friend class hpx::serialization::access;

            template<typename Archive>
            void serialize(Archive& archive, [[maybe_unused]] unsigned int const version)
            {
                archive& _indp;
            }

        private:

            InputNoDataPolicy _indp;
    };


    namespace detail {

        template<typename InputNoDataPolicy_>
        class TypeTraits<InputPolicies<InputNoDataPolicy_>>
        {

            public:

                using InputNoDataPolicy = InputNoDataPolicy_;

                using Element = ElementT<InputNoDataPolicy>;

                template<typename Element>
                using Policies = InputPolicies<InputNoDataPolicyT<InputNoDataPolicy, Element>>;
        };

    }  // namespace detail


    template<typename InputNoDataPolicy, typename HaloPolicy>
    class SpatialOperationInputPolicies: public InputPolicies<InputNoDataPolicy>
    {

        public:

            SpatialOperationInputPolicies():

                InputPolicies<InputNoDataPolicy>{},
                _hp{}

            {
            }

            SpatialOperationInputPolicies(InputNoDataPolicy const& indp):

                InputPolicies<InputNoDataPolicy>{indp},
                _hp{}

            {
            }

            SpatialOperationInputPolicies(HaloPolicy const& hp):

                InputPolicies<InputNoDataPolicy>{},
                _hp{hp}

            {
            }

            SpatialOperationInputPolicies(InputNoDataPolicy const& indp, HaloPolicy const& hp):

                InputPolicies<InputNoDataPolicy>{indp},
                _hp{hp}

            {
            }

            HaloPolicy const& halo_policy() const
            {
                return _hp;
            }

        private:

            friend class hpx::serialization::access;

            template<typename Archive>
            void serialize(Archive& archive, [[maybe_unused]] unsigned int const version)
            {
                InputPolicies<InputNoDataPolicy>::serialize(archive, version);
                archive& _hp;
            }

            HaloPolicy _hp;
    };


    namespace detail {

        template<typename InputNoDataPolicy_, typename HaloPolicy>
        class TypeTraits<SpatialOperationInputPolicies<InputNoDataPolicy_, HaloPolicy>>
        {

            public:

                using InputNoDataPolicy = InputNoDataPolicy_;

                using Element = ElementT<InputNoDataPolicy>;

                template<typename Element>
                using Policies = SpatialOperationInputPolicies<
                    InputNoDataPolicyT<InputNoDataPolicy, Element>,
                    HaloPolicyT<HaloPolicy, Element>>;
        };

    }  // namespace detail

}  // namespace lue::policy

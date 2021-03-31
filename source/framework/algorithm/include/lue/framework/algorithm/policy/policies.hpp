#pragma once
#include "lue/framework/algorithm/policy/detail/type_list.hpp"
#include "lue/framework/algorithm/policy/policy_traits.hpp"
#include <hpx/serialization.hpp>


namespace lue::policy {

    template<
        typename... InputPolicy>
    using InputsPolicies = detail::TypeList<InputPolicy...>;


    template<
        typename... OutputPolicy>
    using OutputsPolicies = detail::TypeList<OutputPolicy...>;


    template<
        typename DomainPolicy,
        typename OutputsPolicies,
        typename InputsPolicies>
    class Policies
    {
    };


    // TODO(KDJ)
    // - output policies:
    //     - OutOfRangePolicy
    //
    // - focal operation
    //     - input policies:
    //         - OutOfImagePolicy
    //     - output policies:
    //         - NoDataFocusElementPolicy


    template<
        typename DomainPolicy,
        typename... OutputPolicy,
        typename... InputPolicy>
    class Policies<
        DomainPolicy,
        detail::TypeList<OutputPolicy...>,
        detail::TypeList<InputPolicy...>>
    {

        public:

            constexpr static std::size_t nr_outputs{sizeof...(OutputPolicy)};
            constexpr static std::size_t nr_inputs{sizeof...(InputPolicy)};

            using OutputsPolicies = std::tuple<OutputPolicy...>;
            using InputsPolicies = std::tuple<InputPolicy...>;

            Policies()=default;

            Policies(
                DomainPolicy const& domain_policy,
                OutputPolicy... output_policies,
                InputPolicy... input_policies):

                _dp{domain_policy},
                _op{output_policies...},
                _ip{input_policies...}

            {
            }

            DomainPolicy const& domain_policy() const
            {
                return _dp;
            }

            InputsPolicies const& inputs_policies() const
            {
                return _ip;
            }

            OutputsPolicies const& outputs_policies() const
            {
                return _op;
            }

        private:

            friend class hpx::serialization::access;

            template<typename Archive>
            void serialize(
                Archive& archive,
                [[maybe_unused]] unsigned int const version)
            {
                archive & _op & _ip;
            }

            DomainPolicy _dp;

            OutputsPolicies _op;

            InputsPolicies _ip;

    };


    namespace detail {

        template<
            typename... P>
        class TypeTraits<
            std::tuple<P ...>>
        {

            public:

                // template<
                //     std::size_t idx,
                //     typename Element>
                // using Policy = typename TypeTraits<typename std::tuple_element<idx, std::tuple<P...>>::type>::
                //     template Type<Element>;

                template<
                    std::size_t idx>
                using Policies = typename std::tuple_element<idx, std::tuple<P...>>::type;

        };


        template<
            typename DomainPolicy_,
            typename OutputsPolicies_,
            typename InputsPolicies_>
        class PoliciesTraits<
            Policies<
                DomainPolicy_,
                OutputsPolicies_,
                InputsPolicies_>>
        {

            public:

                using DomainPolicy = DomainPolicy_;
                using OutputsPolicies = OutputsPolicies_;
                using InputsPolicies = InputsPolicies_;

        };

    }  // namespace detail
}  // namespace lue::policy

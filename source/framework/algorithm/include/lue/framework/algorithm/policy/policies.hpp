#pragma once
#include "lue/framework/algorithm/policy/detail/type_list.hpp"
#include "lue/framework/algorithm/policy/policy_traits.hpp"
#include <hpx/serialization.hpp>


namespace lue::policy {

    template<typename... InputPolicy>
    using InputsPolicies = detail::TypeList<InputPolicy...>;


    template<typename... OutputPolicy>
    using OutputsPolicies = detail::TypeList<OutputPolicy...>;


    template<typename... Element>
    using Elements = detail::TypeList<Element...>;


    /*!
        @brief      Primary class template for a class aggregating all algorithm policies
        @tparam     DomainPolicy_ Domain policy class
        @tparam     OutputsPolicies_ Policies related to zero or more outputs
        @tparam     InputsPolicies_ Policies related to zero or more inputs
    */
    template<typename DomainPolicy_, typename OutputsPolicies_, typename InputsPolicies_>
    class Policies
    {

        public:

            using DomainPolicy = DomainPolicy_;

            using OutputsPolicies = OutputsPolicies_;

            using InputsPolicies = InputsPolicies_;

            /*!
                @brief      Return the domain policy
            */
            auto domain_policy() const -> DomainPolicy const&;

            /*!
                @brief      Return the inputs policies
            */
            auto inputs_policies() const -> InputsPolicies const&;

            /*!
                @brief      Return the outputs policies
            */
            auto outputs_policies() const -> OutputsPolicies const&;
    };


    // TODO(KDJ)
    // - focal operation
    //     - input policies:
    //         - OutOfImagePolicy
    //     - output policies:
    //         - NoDataFocusElementPolicy


    /*!
        @brief      Partial specialization of a class template for a class aggregating all algorithm policies
        @tparam     DomainPolicy_ Domain policy class
        @tparam     OutputsPolicies_ Policies related to the algorithm's output(s)
        @tparam     InputsPolicies_ Policies related to the algorithm's input(s)

        This specialization uses detail::TypeList to be able to separate between an unknown number of output
        policies and an unknown number of input policies.
    */
    template<typename DomainPolicy_, typename... OutputPolicy, typename... InputPolicy>
    class Policies<DomainPolicy_, detail::TypeList<OutputPolicy...>, detail::TypeList<InputPolicy...>>
    {

        public:

            constexpr static std::size_t nr_outputs{sizeof...(OutputPolicy)};
            constexpr static std::size_t nr_inputs{sizeof...(InputPolicy)};

            using DomainPolicy = DomainPolicy_;
            using OutputsPolicies = std::tuple<OutputPolicy...>;
            using InputsPolicies = std::tuple<InputPolicy...>;

            Policies() = default;

            Policies(
                DomainPolicy const& domain_policy,
                OutputPolicy const&... output_policies,
                InputPolicy const&... input_policies):

                _dp{domain_policy},
                _op{output_policies...},
                _ip{input_policies...}

            {
            }

            auto domain_policy() const -> DomainPolicy const&
            {
                return _dp;
            }

            auto inputs_policies() const -> InputsPolicies const&
            {
                return _ip;
            }

            auto outputs_policies() const -> OutputsPolicies const&
            {
                return _op;
            }

        private:

            friend class hpx::serialization::access;

            template<typename Archive>
            void serialize(Archive& archive, [[maybe_unused]] unsigned int const version)
            {
                archive& _op& _ip;
            }

            DomainPolicy _dp;

            OutputsPolicies _op;

            InputsPolicies _ip;
    };


    namespace detail {

        template<typename... P>
        class TypeTraits<std::tuple<P...>>
        {

            public:

                // template<
                //     std::size_t idx,
                //     typename Element>
                // using Policy = typename TypeTraits<typename std::tuple_element<idx,
                // std::tuple<P...>>::type>::
                //     template Type<Element>;

                template<std::size_t idx>
                using Policies = typename std::tuple_element<idx, std::tuple<P...>>::type;
        };

    }  // namespace detail
}  // namespace lue::policy

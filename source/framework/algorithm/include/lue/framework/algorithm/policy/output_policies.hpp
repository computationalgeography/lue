#pragma once
#include "lue/framework/algorithm/policy/policy_traits.hpp"
#include <hpx/serialization.hpp>


namespace lue {
namespace policy {

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
    typename OutputNoDataPolicy>
class TypeTraits<
    OutputPolicies<OutputNoDataPolicy>>
{

    public:

        using Element = ElementT<OutputNoDataPolicy>;

        template<
            typename Element>
        using Policies = OutputPolicies<
            OutputNoDataPolicyT<OutputNoDataPolicy, Element>>;

};

}  // namespace detail
}  // namespace policy
}  // namespace lue

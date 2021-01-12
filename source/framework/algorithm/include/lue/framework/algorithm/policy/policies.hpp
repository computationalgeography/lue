#pragma once
#include "lue/framework/algorithm/policy/detail/type_list.hpp"
#include <hpx/serialization.hpp>


namespace lue {
namespace policy {

template<
    typename... InputPolicy>
using InputsPolicies = detail::TypeList<InputPolicy...>;


template<
    typename... InputPolicy>
using OutputsPolicies = detail::TypeList<InputPolicy...>;


template<
    typename OutputsPolicies,
    typename InputsPolicies>
class Policies
{
};


// TODO(KDJ)
// - operation policies:
//     - OutOfDomainPolicy
// - output policies:
//     - OutOfRangePolicy
//
// - focal operation
//     - input policies:
//         - OutOfImagePolicy
//     - output policies:
//         - NoDataFocusElementPolicy


template<
    typename... OutputPolicy,
    typename... InputPolicy>
class Policies<
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
                OutputPolicy... output_policies,
                InputPolicy... input_policies):

            _op{output_policies...},
            _ip{input_policies...}

        {
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

        OutputsPolicies _op;

        InputsPolicies _ip;

};

}  // namespace policy
}  // namespace lue

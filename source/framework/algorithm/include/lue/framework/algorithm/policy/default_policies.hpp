#pragma once
#include "lue/framework/algorithm/policy/dont_mark_no_data.hpp"
#include "lue/framework/algorithm/policy/fill_halo_with_constant_value.hpp"
#include "lue/framework/algorithm/policy/input_policies.hpp"
#include "lue/framework/algorithm/policy/skip_no_data.hpp"
#include <hpx/serialization.hpp>
#include <array>


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


namespace lue {
namespace policy {
namespace detail {

template<
    typename...>
class TypeList
{
};

}  // namespace detail


template<
    typename... Elements>
using InputElements = detail::TypeList<Elements...>;


template<
    typename... Elements>
using OutputElements = detail::TypeList<Elements...>;


template<
    typename OutputElements,
    typename InputElements>
class DefaultPolicies
{
};


/*!
    @brief      Default policies to use by algorithms

    Defaults:
    - Input no-data policy does not check for no-data in the input
    - Output no-data policy does not mark no-data in the output
*/
template<
    typename... OutputElements,
    typename... InputElements>
class DefaultPolicies<
    detail::TypeList<OutputElements...>,
    detail::TypeList<InputElements...>>
{
    public:

        constexpr static std::size_t nr_outputs{sizeof...(OutputElements)};
        constexpr static std::size_t nr_inputs{sizeof...(InputElements)};

        using OutputNoDataPolicy = DontMarkNoData;
        using OutputNoDataPolicies = std::array<OutputNoDataPolicy, nr_outputs>;

        using InputNoDataPolicy = SkipNoData;

        using InputNoDataPolicies = std::array<InputNoDataPolicy, nr_inputs>;

        InputNoDataPolicies const& input_no_data_policies() const
        {
            return _indp;
        }

        OutputNoDataPolicies const& output_no_data_policies() const
        {
            return _ondp;
        }

    private:

        // All arguments passed to actions have to support serialization,
        // even if there is nothing to serialize
        friend class hpx::serialization::access;

        template<typename Archive>
        void serialize(
            [[maybe_unused]] Archive& archive,
            [[maybe_unused]] unsigned int const version)
        {
            // Nothing to do
        }

        //! For each input a no-data policy
        InputNoDataPolicies _indp;

        //! For each output a no-data policy
        OutputNoDataPolicies _ondp;

};


template<
    typename OutputElements,
    typename InputElements>
class OperationPolicies
{
};


template<
    typename... OutputElements,
    typename... InputElements>
class OperationPolicies<
    detail::TypeList<OutputElements...>,
    detail::TypeList<InputElements...>>
{

    public:

        constexpr static std::size_t nr_outputs{sizeof...(OutputElements)};
        constexpr static std::size_t nr_inputs{sizeof...(InputElements)};

};


template<
    typename OutputElements,
    typename InputElements>
class FocalOperationPolicies
{
};


template<
    typename... OutputElements,
    typename... InputElements>
class FocalOperationPolicies<
    detail::TypeList<OutputElements...>,
    detail::TypeList<InputElements...>>:

    public OperationPolicies<
        detail::TypeList<OutputElements...>,
        detail::TypeList<InputElements...>>

{

};


template<
    typename OutputElements,
    typename InputElements>
class DefaultFocalOperationPolicies
{
};


template<
    typename... OutputElements,
    typename... InputElements>
class DefaultFocalOperationPolicies<
        detail::TypeList<OutputElements...>,
        detail::TypeList<InputElements...>>:
    public FocalOperationPolicies<
        detail::TypeList<OutputElements...>,
        detail::TypeList<InputElements...>>
{

    public:

        using Base = FocalOperationPolicies<
            detail::TypeList<OutputElements...>, detail::TypeList<InputElements...>>;

        using OutputNoDataPolicy = DontMarkNoData;
        using OutputNoDataPolicies = std::array<OutputNoDataPolicy, Base::nr_outputs>;

        using InputNoDataPolicy = SkipNoData;

        template<
            typename InputElement>
        using HaloValuePolicy = FillHaloWithConstantValue<InputElement>;

        // A tuple of, for each input argument, useful policies
        using InputPolicies = std::tuple<
            policy::FocalOperationInputPolicies<
                InputNoDataPolicy, FillHaloWithConstantValue<InputElements>>...>;

        DefaultFocalOperationPolicies(
            InputElements const... fill_values):

            _ondp{},
            _ip{HaloValuePolicy<InputElements>{fill_values}...}

        {
        }

        OutputNoDataPolicies const& output_no_data_policies() const
        {
            return _ondp;
        }

        InputPolicies const& input_policies() const
        {
            return _ip;
        }

    private:

        // All arguments passed to actions have to support serialization,
        // even if there is nothing to serialize
        friend class hpx::serialization::access;

        template<typename Archive>
        void serialize(
            [[maybe_unused]] Archive& archive,
            [[maybe_unused]] unsigned int const version)
        {
            // Nothing to do
        }

        //! For each output a no-data policy
        OutputNoDataPolicies _ondp;

        //! For each input the policies
        InputPolicies _ip;

};

}  // namespace policy
}  // namespace lue

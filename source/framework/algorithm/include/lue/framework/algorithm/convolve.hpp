#pragma once
#include "lue/framework/algorithm/policy/all_values_within_domain.hpp"
#include "lue/framework/algorithm/policy/default_policies.hpp"
#include "lue/framework/partitioned_array.hpp"


namespace lue {
    namespace policy::convolve {

        template<
            typename OutputElement,
            typename InputElement>
        using DefaultPolicies = policy::DefaultSpatialOperationPolicies<
            AllValuesWithinDomain<InputElement>,
            OutputElements<OutputElement>,
            InputElements<InputElement>>;

    }  // namespace policy::convolve


    // template<
    //     typename OutOfImagePolicy_=SkipOutOfImage,
    //     typename NoDataFocusElementPolicy_=KeepNoDataFocusElement
    // >
    // class ConvolvePolicies:
    //     public OutOfImagePolicy_,
    //     public NoDataFocusElementPolicy_
    // {
    //
    // public:
    //
    //     using OutOfImagePolicy = OutOfImagePolicy_;
    //
    //     using NoDataFocusElementPolicy = NoDataFocusElementPolicy_;
    //
    //     ConvolvePolicies() {};
    //
    // private:
    //
    // };


    template<
        typename OutputElement,
        typename ConvolvePolicies,
        typename Array,
        typename Kernel>
    PartitionedArrayT<Array, OutputElement> convolve(
        ConvolvePolicies const& policies,
        Array const& array,
        Kernel const& kernel);


    template<
        typename OutputElement,
        typename Array,
        typename Kernel>
    PartitionedArrayT<Array, OutputElement> convolve(
        Array const& array,
        Kernel const& kernel)
    {
        using InputElement = ElementT<Array>;
        using Policies = policy::convolve::DefaultPolicies<OutputElement, InputElement>;

        InputElement const fill_value{0};

        return convolve<OutputElement>(Policies{fill_value}, array, kernel);
    }

}  // namespace lue

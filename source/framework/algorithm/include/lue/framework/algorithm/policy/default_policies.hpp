#pragma once
#include "lue/framework/algorithm/policy/dont_mark_no_data.hpp"
#include "lue/framework/algorithm/policy/skip_no_data.hpp"
#include <hpx/serialization.hpp>
#include <array>


namespace lue {
namespace policy {

/*!
    @brief      Default policies to use by algorithms

    Defaults:
    - Input no-data policy does not check for no-data in the input
    - Output no-data policy does not mark no-data in the output
*/
template<
    std::size_t nr_inputs,
    std::size_t nr_outputs=1>
class DefaultPolicies
{
    public:

        using InputNoDataPolicy = SkipNoData;
        using OutputNoDataPolicy = DontMarkNoData;

        using InputNoDataPolicies = std::array<InputNoDataPolicy, nr_inputs>;
        using OutputNoDataPolicies = std::array<OutputNoDataPolicy, nr_outputs>;

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

}  // namespace policy
}  // namespace lue

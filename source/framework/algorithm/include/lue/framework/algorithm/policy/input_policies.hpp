#pragma once
#include <hpx/serialization.hpp>


namespace lue {
namespace policy {

template<
    typename InputNoDataPolicy>
class InputPolicies
{

    public:

        InputPolicies():

            _indp{}

        {
        }

        InputPolicies(
            InputNoDataPolicy const& indp):

            _indp{indp}

        {
        }

        InputNoDataPolicy const& input_no_data_policy() const
        {
            return _indp;
        }

    private:

        friend class hpx::serialization::access;

        template<typename Archive>
        void serialize(
            Archive& archive,
            [[maybe_unused]] unsigned int const version)
        {
            archive & _indp;
        }

        InputNoDataPolicy _indp;

};


template<
    typename InputNoDataPolicy,
    typename HaloValuePolicy>
class FocalOperationInputPolicies:
    InputPolicies<InputNoDataPolicy>
{

    public:

        FocalOperationInputPolicies(
            HaloValuePolicy const& hvp):

            InputPolicies<InputNoDataPolicy>{},
            _hvp{hvp}

        {
        }

        FocalOperationInputPolicies(
            InputNoDataPolicy const& indp,
            HaloValuePolicy const& hvp):

            InputPolicies<InputNoDataPolicy>{indp},
            _hvp{hvp}

        {
        }

        HaloValuePolicy const& halo_value_policy() const
        {
            return _hvp;
        }

    private:

        HaloValuePolicy _hvp;

};

}  // namespace policy
}  // namespace lue

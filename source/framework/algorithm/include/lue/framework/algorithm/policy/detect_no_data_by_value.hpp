#pragma once
#include "lue/framework/algorithm/policy/policy_traits.hpp"
#include <hpx/serialization.hpp>
#include <type_traits>


namespace lue {
namespace policy {

template<
    typename Element>
class DetectNoDataByValue
{

    public:

        DetectNoDataByValue():

            DetectNoDataByValue(no_data_value<Element>)

        {
        }

        DetectNoDataByValue(
                Element const value):

            _value{value}

        {
        }

        bool is_no_data(
            Element const& element) const
        {
            return element == _value;
        }

        template<
            typename Data>
        bool is_no_data(
            Data const& data,
            Index const idx) const
        {
            static_assert(std::is_same_v<lue::ElementT<Data>, Element>);

            return data[idx] == _value;
        }

        template<
            typename Data,
            typename... Idxs>
        bool is_no_data(
            Data const& data,
            Idxs const... idxs) const
        {
            static_assert(std::is_same_v<lue::ElementT<Data>, Element>);

            return data(idxs...) == _value;
        }

    protected:

        Element value() const
        {
            return _value;
        }

    private:

        friend class hpx::serialization::access;

        template<typename Archive>
        void serialize(
            Archive& archive,
            [[maybe_unused]] unsigned int const version)
        {
            archive & _value;
        }

        Element _value;

};


namespace detail {

template<
    typename E>
class TypeTraits<
    DetectNoDataByValue<E>>
{

    public:

        using Element = E;

        template<
            typename E_>
        using Policy = DetectNoDataByValue<E_>;

};

}  // namespace detail
}  // namespace policy
}  // namespace lue

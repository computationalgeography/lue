#pragma once
#include "lue/framework/core/type_traits.hpp"
#include <hpx/serialization.hpp>
#include <type_traits>


namespace lue {
namespace policy {

template<
    typename Element>
class DetectNoDataByValue
{

    public:

        DetectNoDataByValue()=default;

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
            typename Data,
            typename... Idxs>
        bool is_no_data(
            Data const& data,
            Idxs const... idxs) const
        {
            static_assert(std::is_same_v<ElementT<Data>, Element>);

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

}  // namespace policy
}  // namespace lue

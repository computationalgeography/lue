#pragma once
#include "lue/framework/core/assert.hpp"
#include "lue/framework/core/type_traits.hpp"
#include <hpx/serialization.hpp>
#include <limits>
#include <type_traits>


namespace lue::policy {

    template<typename Element>
    class DetectNoDataByValue
    {

        public:

            static constexpr Element no_data_value{
                []()
                {
                    static_assert(std::is_floating_point_v<Element> || std::is_integral_v<Element>);

                    if constexpr (std::is_integral_v<Element> && std::is_signed_v<Element>)
                    {
                        return std::numeric_limits<Element>::min();
                    }
                    else if constexpr (std::is_integral_v<Element> && std::is_unsigned_v<Element>)
                    {
                        return std::numeric_limits<Element>::max();
                    }
                    else if constexpr (std::is_floating_point_v<Element>)
                    {
                        return std::numeric_limits<Element>::lowest();
                    }
                }()};


            DetectNoDataByValue():

                DetectNoDataByValue{no_data_value}

            {
            }


            DetectNoDataByValue(Element const value):

                _value{value}

            {
                if constexpr (std::is_floating_point_v<Element>)
                {
                    // This class compares the layered value with values
                    // passed in. This will not work when the layered value is
                    // a NaN. Use another policy if you want to compare
                    // with NaN.
                    lue_hpx_assert(!std::isnan(value));
                }
            }


            template<typename Data>
            bool is_no_data(Data const& data, Index const idx) const
            {
                static_assert(std::is_same_v<lue::ElementT<Data>, Element>);

                return data[idx] == _value;
            }


            template<typename Data, typename... Idxs>
            bool is_no_data(Data const& data, Idxs const... idxs) const
            {
                if constexpr (sizeof...(Idxs) == 0)
                {
                    static_assert(std::is_arithmetic_v<Data>);

                    return data == _value;
                }
                else
                {
                    static_assert(std::is_same_v<lue::ElementT<Data>, Element>);

                    return data(idxs...) == _value;
                }
            }


        protected:

            Element value() const
            {
                return _value;
            }


        private:

            friend class hpx::serialization::access;


            template<typename Archive>
            void serialize(Archive& archive, [[maybe_unused]] unsigned int const version)
            {
                archive& _value;
            }


            Element _value;
    };

}  // namespace lue::policy

#pragma once
#include "lue/framework/core/assert.hpp"
#include "lue/framework/core/type_traits.hpp"
#include <hpx/serialization.hpp>
#include <limits>
#include <type_traits>


namespace lue::policy {

    /*!
        @brief      Input no-data policy
        @warning    Don't use this policy when @a Element is a floating point type and when the no-data value
                    must be represented by a NaN
        @sa         MarkNoDataByValue, DetectNoDataByNaN

        Policy suitable for those cases where no-data is represented by some "special" value, like a minimum
        value, maximum value, or a very small value.
    */
    template<typename Element>
    class DetectNoDataByValue
    {

        public:

            /*!
                @brief      The default no-data value to use in tests

                These values are used, depending on @a Element:

                | Type | value |
                | ---  | ---   |
                | signed integral | std::numeric_limits<Element>::min() |
                | unsigned integral | std::numeric_limits<Element>::max() |
                | floating point | std::numeric_limits<Element>::lowest() |
            */
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


            /*!
                @brief      Construct an instance, using the default DetectNoDataByValue::no_data_value as
                            the no-data value in tests
            */
            DetectNoDataByValue():

                DetectNoDataByValue{no_data_value}

            {
            }


            /*!
                @brief      Construct an instance, using @a value as the no-data value in tests
                @warning    If @a Element is a floating point type, @a value must not by NaN
            */
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


            /*!
                @brief      Return whether @a value is no-data
            */
            auto is_no_data(Element const& value) const -> bool
            {
                return value == _value;
            }


            /*!
                @brief      Return whether the element at index @a idx in @a data contains no-data
                @tparam     Data Collection of elements
            */
            template<typename Data>
            auto is_no_data(Data const& data, Index const idx) const -> bool
            {
                static_assert(std::is_same_v<lue::ElementT<Data>, Element>);

                return data[idx] == _value;
            }


            /*!
                @brief      Return whether the element at index @a idx in @a data contains no-data
                @tparam     Data Collection of elements
            */
            template<typename Data, typename... Idxs>
            auto is_no_data(Data const& data, Idxs const... idxs) const -> bool
            {
                static_assert(std::is_same_v<lue::ElementT<Data>, Element>);

                return data(idxs...) == _value;
            }


        protected:

            auto value() const -> Element
            {
                return _value;
            }


        private:

            friend class hpx::serialization::access;


            template<typename Archive>
            void serialize(Archive& archive, [[maybe_unused]] unsigned int const version)
            {
                // clang-format off
                archive & _value;
                // clang-format on
            }


            Element _value;
    };

}  // namespace lue::policy

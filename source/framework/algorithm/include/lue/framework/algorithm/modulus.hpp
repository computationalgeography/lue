/*--------------------------------------------OK run
well--------------------------------------------------------------------------- #pragma once #include
"lue/framework/algorithm/binary_local_operation.hpp" #include "lue/framework/algorithm/operator.hpp" #include
"lue/framework/algorithm/policy.hpp" #include <type_traits>


namespace lue {
    namespace detail {

        template<typename InputElement, typename OutputElement_ = InputElement>
        class Modulus
        {

            public:

                using OutputElement = OutputElement_;


                constexpr OutputElement operator()(
                    InputElement const& input_element1, InputElement const& input_element2) const noexcept
                {

                    if constexpr (std::is_integral_v<InputElement>)
                    {
                        return input_element1 % input_element2;
                    }
                    else
                    {
                        return 1;
                    }
                }
        };

    }  // namespace detail


    LUE_BINARY_LOCAL_OPERATION_OVERLOADS3(modulus, detail::Modulus)

}  // namespace lue

----------------------------------------------------------------------------------------------------------------------------*/


#pragma once
#include "lue/framework/algorithm/binary_local_operation.hpp"
#include "lue/framework/algorithm/operator.hpp"
#include "lue/framework/algorithm/policy.hpp"
#include <type_traits>

namespace lue {
    namespace detail {

        template<typename InputElement, typename OutputElement_ = InputElement>
        class Modulus
        {
            public:

                using OutputElement = OutputElement_;

                constexpr OutputElement operator()(
                    InputElement const& input_element1, InputElement const& input_element2) const noexcept
                {
                    static_assert(
                        std::is_integral_v<InputElement>,
                        "Input arguments for modulus must be an integral type");

                    return input_element1 % input_element2;
                }
        };

    }  // namespace detail

    LUE_BINARY_LOCAL_OPERATION_OVERLOADS3(modulus, detail::Modulus)

}  // namespace lue


/*
#pragma once

#include "lue/framework/algorithm/binary_local_operation.hpp"
#include "lue/framework/algorithm/operator.hpp"
#include "lue/framework/algorithm/policy.hpp"
#include <type_traits>

namespace lue {
    namespace detail {

        template<typename InputElement, typename OutputElement_ = InputElement, typename Enable = void>
        class Modulus;

        template<typename InputElement, typename OutputElement_>
        class Modulus<InputElement, OutputElement_, std::enable_if_t<std::is_integral_v<InputElement>>>
        {
            public:

                using OutputElement = OutputElement_;

                constexpr OutputElement operator()(
                    InputElement const& input_element1, InputElement const& input_element2) const
                {
                    if (input_element2 == 0)
                    {
                        throw std::invalid_argument("Division by zero in modulus operation");
                    }
                    return input_element1 % input_element2;
                }
        };

    }  // namespace detail

    LUE_BINARY_LOCAL_OPERATION_OVERLOADS3(modulus, detail::Modulus)

}  // namespace lue

*/

/*----------------------------------------------------------------------------------
#pragma once
#include "lue/framework/algorithm/binary_local_operation.hpp"
#include "lue/framework/algorithm/operator.hpp"
#include "lue/framework/algorithm/policy.hpp"
#include <type_traits>

namespace lue {
    namespace detail {

        template<typename InputElement, typename OutputElement_ = InputElement>
        class Modulus
        {
            public:

                using OutputElement = OutputElement_;

                constexpr OutputElement operator()(
                    InputElement const& input_element1, InputElement const& input_element2) const noexcept
                {
                    static_assert(
                        std::is_integral_v<InputElement>,
                        "Input arguments for modulus must be an integral type");

                    return input_element1 % input_element2;
                }
        };

    }  // namespace detail

    LUE_BINARY_LOCAL_OPERATION_OVERLOADS3(modulus, detail::Modulus)

}  // namespace lue

*---------------------------------------------------------------------------------------------/

/*---------------------------------------------------------------------------------------------------------------------------------
#pragma once
#include "lue/framework/algorithm/binary_local_operation.hpp"
#include "lue/framework/algorithm/operator.hpp"
#include "lue/framework/algorithm/policy.hpp"
#include <cmath>
#include <type_traits>


namespace lue {
    namespace detail {

        template<typename InputElement, typename OutputElement_ = InputElement>
        class Modulus
        {

            public:

                using OutputElement = OutputElement_;


                constexpr OutputElement operator()(
                    InputElement const& input_element1, InputElement const& input_element2) const noexcept
                {

                    if constexpr (std::is_integral_v<InputElement>)
                    {
                        return input_element1 % input_element2;
                    }
                    else
                    {
                        static_assert(
                            std::is_arithmetic_v<InputElement>, "Modulus operation requires integer types");
                    }
                }
        };

    }  // namespace detail


    LUE_BINARY_LOCAL_OPERATION_OVERLOADS3(modulus, detail::Modulus)

}  // namespace lue

--------------------------------------------------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------------------------------------------

#pragma once
#include "lue/framework/algorithm/binary_local_operation.hpp"
#include "lue/framework/algorithm/operator.hpp"
#include "lue/framework/algorithm/policy.hpp"
#include <cmath>
#include <type_traits>


namespace lue {
    namespace detail {

        template<typename InputElement, typename OutputElement_ = InputElement>
        class Modulus
        {
                static_assert(
                    std::is_integral_v<InputElement>, "Input arguments for modulus must be an integral type");

            public:

                using OutputElement = OutputElement_;

                constexpr OutputElement operator()(
                    InputElement const& input_element1, InputElement const& input_element2) const
                {

                    if (input_element2 == 0)
                    {
                        throw std::invalid_argument("Division by zero in modulus operation");
                    }

                    return input_element1 % input_element2;
                }
        };

    }  // namespace detail


    LUE_BINARY_LOCAL_OPERATION_OVERLOADS3(modulus, detail::Modulus)

}  // namespace lue




-----------------------------------------------------------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------------------------------------------------------------

namespace lue {
    namespace detail {

        template<typename InputElement, typename OutputElement_ = InputElement>
        class Modulus
        {

            public:

                using OutputElement = OutputElement_;


                constexpr OutputElement operator()(
                    InputElement const& input_element1, InputElement const& input_element2) const noexcept
                {

                    if constexpr (std::is_integral_v<InputElement>)
                    {
                        return input_element1 % input_element2;
                    }
                    else
                    {
                        static_assert(
                            std::is_arithmetic_v<InputElement>, "Modulus operation requires integer types");
                    }
                }
        };

    }  // namespace detail


    LUE_BINARY_LOCAL_OPERATION_OVERLOADS3(modulus, detail::Modulus)

}  // namespace lue





---------------------------------------------------------------------------------------------------------------------------------*/
/*-------------------------------------------------initial one
works-------------------------------------------------------------------------------

#pragma once
#include "lue/framework/algorithm/binary_local_operation.hpp"
#include "lue/framework/algorithm/operator.hpp"
#include "lue/framework/algorithm/policy.hpp"
#include <cmath>
#include <type_traits>


namespace lue {
    namespace detail {

        template<typename InputElement, typename OutputElement_ = InputElement>
        class Modulus
        {

            public:

                using OutputElement = OutputElement_;


                constexpr OutputElement operator()(
                    InputElement const& input_element1, InputElement const& input_element2) const noexcept
                {

                    if constexpr (std::is_integral_v<InputElement>)
                    {
                        return input_element1 % input_element2;
                    }
                    else if constexpr (std::is_floating_point_v<InputElement>)
                    {
                        return std::fmod(input_element1, input_element2);
                    }
                    else
                    {
                        static_assert(
                            std::is_arithmetic_v<InputElement>,
                            "Modulus operation requires arithmetic types");
                    }
                }
        };

    }  // namespace detail


    LUE_BINARY_LOCAL_OPERATION_OVERLOADS3(modulus, detail::Modulus)

}  // namespace lue








------------------------------------------------------------------------------------------------------------------------------------*/
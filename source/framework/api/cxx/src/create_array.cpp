#include "lue/framework/api/cxx/create_array.hpp"
#include "detail/unsupported_overload.hpp"
#include "overload.hpp"
#include "lue/framework/algorithm/create_partitioned_array.hpp"


namespace lue {

    // TODO These should not be needed. Add support for all overloads to algorithm/create_partitioned_array

    auto create_partitioned_array(
        [[maybe_unused]] auto const& array_shape,
        [[maybe_unused]] auto const& partition_shape,
        auto const& fill_value) -> api::Field
    {
        api::detail::unsupported_overload("create_array", array_shape, partition_shape, fill_value);

        return {};
    }


    auto create_partitioned_array([[maybe_unused]] auto const& array_shape, auto const& fill_value)
        -> api::Field
    {
        api::detail::unsupported_overload("create_array", array_shape, fill_value);

        return {};
    }


    namespace api {

        // TODO create_partitioned_array must support Scalar<Element> const& fill_value argument

        /*!
            @brief      Create an array filled with a scalar value
        */
        auto create_array(
            Shape<Count, 2> const& array_shape,
            Shape<Count, 2> const& partition_shape,
            Field const& fill_value) -> Field
        {
            return std::visit(
                overload{[&array_shape, &partition_shape](auto const& fill_value) -> Field {
                    return create_partitioned_array(array_shape, partition_shape, fill_value);
                }},
                fill_value);
        }


        /*!
            @overload
        */
        auto create_array(Shape<Count, 2> const& array_shape, Field const& fill_value) -> Field
        {
            return std::visit(
                overload{[&array_shape](auto const& fill_value) -> Field {
                    return create_partitioned_array(array_shape, fill_value);
                }},
                fill_value);
        }

    }  // namespace api
}  // namespace lue

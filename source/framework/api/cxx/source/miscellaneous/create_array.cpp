#include "lue/framework/api/cxx/miscellaneous/create_array.hpp"
#include "lue/framework/algorithm/create_partitioned_array.hpp"
#include "lue/framework/api/cxx/detail/unsupported_overload.hpp"
#include "lue/overload.hpp"


namespace lue {

    auto create_partitioned_array(
        auto const& array_shape, auto const& partition_shape, auto const& fill_value) -> api::Array
    {
        api::detail::unsupported_overload("create_array", array_shape, partition_shape, fill_value);

        return {};
    }


    auto create_partitioned_array(auto const& array_shape, auto const& fill_value) -> api::Array
    {
        api::detail::unsupported_overload("create_array", array_shape, fill_value);

        return {};
    }


    namespace api {

        /*!
            @brief      Create an array filled with a scalar value
        */
        auto create_array(
            Shape<Count, 2> const& array_shape,
            Shape<Count, 2> const& partition_shape,
            Scalar const& fill_value) -> Array
        {
            return std::visit(
                overload{
                    [&array_shape, &partition_shape](auto const& fill_value) -> Array
                    { return create_partitioned_array(array_shape, partition_shape, fill_value); }},
                fill_value.variant());
        }


        /*!
            @overload
        */
        auto create_array(Shape<Count, 2> const& array_shape, Scalar const& fill_value) -> Array
        {
            return std::visit(
                overload{
                    [&array_shape](auto const& fill_value) -> Array
                    { return create_partitioned_array(array_shape, fill_value); }},
                fill_value.variant());
        }

    }  // namespace api
}  // namespace lue

#include "lue/framework/api/cxx/create_array.hpp"
#include "detail/unsupported_overload.hpp"
#include "overload.hpp"
#include "lue/framework/algorithm/create_partitioned_array.hpp"


namespace lue {

    auto create_partitioned_array(
        [[maybe_unused]] auto const& array_shape,
        [[maybe_unused]] auto const& partition_shape,
        auto const& fill_value) -> api::Array
    {
        api::detail::unsupported_overload("create_array", fill_value);

        return {};
    }


    auto create_partitioned_array([[maybe_unused]] auto const& array_shape, auto const& fill_value)
        -> api::Array
    {
        api::detail::unsupported_overload("create_array", fill_value);

        return {};
    }


    namespace api {

        auto create_array(
            Shape<Count, 2> const& array_shape,
            Shape<Count, 2> const& partition_shape,
            Scalar const& fill_value) -> Array
        {
            return std::visit(
                overload{[&array_shape, &partition_shape](auto const& fill_value) -> Array {
                    return create_partitioned_array(array_shape, partition_shape, fill_value);
                }},
                fill_value);
        }


        auto create_array(Shape<Count, 2> const& array_shape, Scalar const& fill_value) -> Array
        {
            return std::visit(
                overload{[&array_shape](auto const& fill_value) -> Array {
                    return create_partitioned_array(array_shape, fill_value);
                }},
                fill_value);
        }

    }  // namespace api
}  // namespace lue

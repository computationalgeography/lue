#include "lue/framework/api/cxx/local/open_simplex_noise.hpp"
#include "detail/unsupported_overload.hpp"
#include "overload.hpp"
#include "lue/framework/algorithm/value_policies/open_simplex_noise.hpp"


namespace lue {
    namespace value_policies {

        auto open_simplex_noise(auto const& x_coordinates, auto const& y_coordinates, int const seed)
            -> api::Field
        {
            api::detail::unsupported_overload("open_simplex_noise", x_coordinates, y_coordinates, seed);

            return {};
        }

    }  // namespace value_policies


    namespace api {

        auto open_simplex_noise(Field const& x_coordinates, Field const& y_coordinates, int const seed)
            -> Field
        {
            return std::visit(
                overload{
                    [seed](auto const& x_coordinates, auto const& y_coordinates) -> Field
                    { return value_policies::open_simplex_noise(x_coordinates, y_coordinates, seed); }},
                x_coordinates.variant(),
                y_coordinates.variant());
        }

    }  // namespace api
}  // namespace lue

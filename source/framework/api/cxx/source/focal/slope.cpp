#include "lue/framework/api/cxx/focal/slope.hpp"
#include "lue/framework/algorithm/value_policies/slope.hpp"
#include "lue/overload.hpp"
#include "lue/framework/api/cxx/detail/unsupported_overload.hpp"


namespace lue {
    namespace value_policies {

        /*!
            @brief      Overload called in case of unsupported arguments
        */
        auto slope(auto const& elevation, auto const& cell_size) -> api::Field
        {
            api::detail::unsupported_overload("slope", elevation, cell_size);

            return {};
        }

    }  // namespace value_policies


    namespace api {

        /*!
            @brief      TODO
        */
        auto slope(Field const& elevation, Literal const& cell_size) -> Field
        {
            return std::visit(
                overload{
                    [](auto const& elevation, auto const& cell_size) -> Field
                    { return value_policies::slope(elevation, cell_size); }},
                elevation.variant(),
                cell_size.variant());
        }

    }  // namespace api
}  // namespace lue

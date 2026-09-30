#include "lue/framework/api/cxx/focal/aspect.hpp"
#include "lue/framework/algorithm/value_policies/aspect.hpp"
#include "lue/framework/api/cxx/detail/unsupported_overload.hpp"
#include "lue/overload.hpp"


namespace lue {
    namespace value_policies {

        /*!
            @brief      Overload called in case of unsupported arguments
        */
        auto aspect(auto const& elevation) -> api::Field
        {
            api::detail::unsupported_overload("aspect", elevation);

            return {};
        }

    }  // namespace value_policies


    namespace api {

        /*!
            @brief      TODO
        */
        auto aspect(Field const& elevation) -> Field
        {
            return std::visit(
                overload{[](auto const& elevation) -> Field { return value_policies::aspect(elevation); }},
                elevation.variant());
        }

    }  // namespace api
}  // namespace lue

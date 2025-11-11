#include "lue/framework/api/cxx/focal/mean.hpp"
#include "detail/unsupported_overload.hpp"
#include "overload.hpp"
#include "lue/framework/algorithm/value_policies/focal_mean.hpp"


namespace lue {
    namespace value_policies {

        /*!
            @brief      Overload called in case of unsupported arguments
        */
        auto focal_mean(auto const& field, auto const& kernel) -> api::Field
        {
            api::detail::unsupported_overload("focal_mean", field, kernel);

            return {};
        }

    }  // namespace value_policies


    namespace api {

        /*!
            @brief      TODO
        */
        auto focal_mean(Field const& field, BooleanKernel const& kernel) -> Field
        {
            return std::visit(
                overload{[](auto const& field, auto const& kernel) -> Field {
                    return value_policies::focal_mean(field, kernel);
                }},
                field.variant(),
                kernel.variant());
        }

    }  // namespace api
}  // namespace lue

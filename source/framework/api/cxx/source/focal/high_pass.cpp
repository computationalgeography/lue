#include "lue/framework/api/cxx/focal/high_pass.hpp"
#include "lue/framework/algorithm/value_policies/focal_high_pass.hpp"
#include "lue/overload.hpp"
#include "lue/framework/api/cxx/detail/unsupported_overload.hpp"


namespace lue {
    namespace value_policies {

        /*!
            @brief      Overload called in case of unsupported arguments
        */
        auto focal_high_pass(auto const& field, auto const& kernel) -> api::Field
        {
            api::detail::unsupported_overload("focal_high_pass", field, kernel);

            return {};
        }

    }  // namespace value_policies


    namespace api {

        /*!
            @brief      TODO
        */
        auto focal_high_pass(Field const& field, Kernel const& kernel) -> Field
        {
            return std::visit(
                overload{
                    [](auto const& field, auto const& kernel) -> Field
                    { return value_policies::focal_high_pass(field, kernel); }},
                field.variant(),
                kernel.variant());
        }

    }  // namespace api
}  // namespace lue

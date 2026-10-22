#include "lue/framework/api/cxx/focal/convolve.hpp"
#include "lue/framework/algorithm/value_policies/convolve.hpp"
#include "lue/framework/api/cxx/detail/overload.hpp"
#include "lue/framework/api/cxx/detail/unsupported_overload.hpp"


namespace lue {
    namespace value_policies {

        /*!
            @brief      Overload called in case of unsupported arguments
        */
        auto convolve(auto const& field, auto const& kernel) -> api::Field
        {
            api::detail::unsupported_overload("convolve", field, kernel);

            return {};
        }

    }  // namespace value_policies


    namespace api {

        /*!
            @brief      TODO
        */
        auto convolve(Field const& field, Kernel const& kernel) -> Field
        {
            return std::visit(
                overload{
                    [](auto const& field, auto const& kernel) -> Field
                    { return value_policies::convolve(field, kernel); }},
                field.variant(),
                kernel.variant());
        }

    }  // namespace api
}  // namespace lue

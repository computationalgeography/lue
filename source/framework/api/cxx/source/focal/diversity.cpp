#include "lue/framework/api/cxx/focal/diversity.hpp"
#include "lue/framework/algorithm/value_policies/focal_diversity.hpp"
#include "lue/overload.hpp"
#include "lue/framework/api/cxx/detail/unsupported_overload.hpp"
#include "lue/framework.hpp"


namespace lue {
    namespace value_policies {

        /*!
            @brief      Overload called in case of unsupported arguments
        */
        template<std::integral Count>
        auto focal_diversity(auto const& field, auto const& kernel) -> api::Field
        {
            api::detail::unsupported_overload("focal_diversity", field, kernel);

            return {};
        }

    }  // namespace value_policies


    namespace api {

        /*!
            @brief      TODO
        */
        auto focal_diversity(Field const& field, Kernel const& kernel) -> Field
        {
            return std::visit(
                overload{
                    [](auto const& field, auto const& kernel) -> Field
                    { return value_policies::focal_diversity<CountElement>(field, kernel); }},
                field.variant(),
                kernel.variant());
        }

    }  // namespace api
}  // namespace lue

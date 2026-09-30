#include "lue/framework/algorithm/value_policies/cell_index.hpp"
#include "lue/overload.hpp"
#include "lue/framework/api/cxx/detail/unsupported_overload.hpp"
#include "lue/framework/api/cxx/miscellaneous/cell_index.hpp"
#include "lue/framework/configure.hpp"


namespace lue {

    namespace value_policies {

        template<typename IndexElement>
        auto cell_index(auto const& condition, Index const dimension_idx) -> api::Field
        {
            api::detail::unsupported_overload("cell_index", condition, dimension_idx);

            return {};
        }

    }  // namespace value_policies


    namespace api {

        auto cell_index(Field const& condition, Index const dimension_idx) -> Field
        {
            return std::visit(
                overload{[dimension_idx](auto const& condition) -> Field {
                    return value_policies::cell_index<IndexElement>(condition, dimension_idx);
                }},
                condition.variant());
        }

    }  // namespace api
}  // namespace lue

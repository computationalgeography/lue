#include "lue/framework/api/cxx/miscellaneous/reclassify.hpp"
#include "lue/framework/algorithm/value_policies/reclassify.hpp"
#include "lue/framework/api/cxx/detail/overload.hpp"
#include "lue/framework/api/cxx/detail/unsupported_overload.hpp"


namespace lue {

    namespace value_policies {

        auto reclassify(auto const& field, auto const& lookup_table) -> api::Field
        {
            api::detail::unsupported_overload("reclassify", field, lookup_table);

            return {};
        }

    }  // namespace value_policies


    namespace api {

        auto reclassify(Field const& field, LookupTable const& lookup_table) -> Field
        {
            return std::visit(
                overload{
                    [](auto const& field, auto const& lookup_table) -> Field
                    { return value_policies::reclassify(field, lookup_table); }},
                field.variant(),
                lookup_table.variant());
        }

    }  // namespace api

}  // namespace lue

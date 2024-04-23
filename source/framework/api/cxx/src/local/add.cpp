#include "lue/framework/api/cxx/local/add.hpp"
#include "detail/unsupported_overload.hpp"
#include "overload.hpp"
#include "lue/framework/algorithm/value_policies/add.hpp"


namespace lue {
    namespace value_policies {

        /*!
            @brief      Overload called in case of unsupported arguments
        */
        auto add(auto const& field1, auto const& field2) -> Field
        {
            detail::unsupported_overload("add", field1, field2);

            return {};
        }

    }  // namespace value_policies


    /*!
        @brief      Add two fields together and return a new field containing the resultin values
    */
    auto add(Field const& field1, Field const& field2) -> Field
    {
        // At compile-time, all combinations of field types are mapped to a suitable overload,
        // by the compiler, using C++ overloading rules. At runtime, the mapped overload for
        // a specific combination of field types is called.
        //
        // The availability / visibility of overloaded functions determines the mapping. This depends on:
        // - namespace visibility (here: value_policies::)
        // - set of overloads (here: those declared in value_policies/add.hpp)
        // - name of function (here: add)
        //
        // → We could maybe make a macro that results in a function (module?) like the
        //   current one, for a family of functions
        return std::visit(
            overload{[](auto const& field1, auto const& field2) -> Field {
                return value_policies::add(field1, field2);
            }},
            field1,
            field2);
    }

}  // namespace lue

#pragma once
#include "detail/type_name.hpp"
#include "lue/framework/api/cxx/field.hpp"
#include <string>


namespace lue::detail {

    void unsupported_overload(std::string const& name, std::string const& field1, std::string const& field2);


    void unsupported_overload(std::string const& name, auto const& field1, auto const& field2)
    {
        unsupported_overload(name, type_name(field1), type_name(field2));
    }

}  // namespace lue::detail

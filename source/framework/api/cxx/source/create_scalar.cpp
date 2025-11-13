#include "lue/framework/api/cxx/create_scalar.hpp"
#include "detail/unsupported_overload.hpp"
#include "lue/concept.hpp"
#include "lue/overload.hpp"


namespace lue {

    template<Arithmetic T>
    auto create_scalar(T const& value) -> lue::Scalar<T>
    {
        return lue::Scalar<T>{value};
    }


    namespace api {

        /*!
            @brief      Create a scalar given a literal value
        */
        auto create_scalar(Literal const& value) -> Field
        {
            return std::visit(
                overload{[](auto const& value) -> Field { return lue::create_scalar(value); }},
                value.variant());
        }

    }  // namespace api
}  // namespace lue

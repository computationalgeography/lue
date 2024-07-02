#include "lue/framework/api/cxx/create_scalar.hpp"
#include "detail/unsupported_overload.hpp"
#include "overload.hpp"
#include "lue/concept.hpp"


namespace lue {

    template<
        Arithmetic T>
    auto create_scalar(T const& fill_value) -> lue::Scalar<T>
    {
        return lue::Scalar<T>{fill_value};
    }


    auto create_scalar(auto const& fill_value) -> api::Scalar
    {
        api::detail::unsupported_overload("create_scalar", fill_value);

        return {};
    }


    namespace api {

        auto create_scalar(Literal const& fill_value) -> Scalar
        {
            return std::visit(
                overload{[](auto const& fill_value) -> Scalar {
                    return lue::create_scalar(fill_value);
                }},
                fill_value);
        }

    }  // namespace api
}  // namespace lue

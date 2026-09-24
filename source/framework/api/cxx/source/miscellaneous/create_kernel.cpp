#include "lue/framework/api/cxx/miscellaneous/create_kernel.hpp"
#include "lue/framework/algorithm/kernel.hpp"
#include "lue/overload.hpp"
#include "lue/framework/api/cxx/detail/unsupported_overload.hpp"


namespace lue {

    auto box_kernel(Count const radius, auto const weight) -> api::Kernel
    {
        api::detail::unsupported_overload("create_box_kernel", radius, weight);

        return {};
    }

    namespace api {

        auto create_box_kernel(Count const radius, Literal const& weight) -> Kernel
        {
            return std::visit(
                overload{
                    [radius](auto const weight) -> Kernel
                    { return box_kernel<std::remove_const_t<decltype(weight)>, 2>(radius, weight); }},
                weight.variant());
        }

    }  // namespace api

}  // namespace lue

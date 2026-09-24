#include "lue/framework/api/cxx/as_state.hpp"
#include "lue/framework/api/cxx/detail/unsupported_overload.hpp"
#include "lue/overload.hpp"


namespace lue {

    auto as_state(auto const& field) -> hpx::shared_future<void>
    {
        api::detail::unsupported_overload("as_state", field);

        return {};
    }


    template<Arithmetic Element, Rank rank>
    auto as_state(PartitionedArray<Element, rank> const& array) -> hpx::shared_future<void>
    {
        return hpx::when_all(array.partitions().begin(), array.partitions().end()).share();
    }


    template<Arithmetic Element>
    auto as_state(Scalar<Element> const& scalar) -> hpx::shared_future<void>
    {
        return scalar.future();
    }


    namespace api {

        auto as_state(Field const& field) -> hpx::shared_future<void>
        {
            return std::visit(
                overload{[](auto const& field) -> hpx::shared_future<void> { return lue::as_state(field); }},
                field.variant());
        }

    }  // namespace api
}  // namespace lue

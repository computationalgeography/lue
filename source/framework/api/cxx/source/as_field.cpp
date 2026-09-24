#include "lue/framework/api/cxx/as_field.hpp"
#include "lue/overload.hpp"


namespace lue {

    template<Arithmetic Element, Rank rank>
    auto as_field(PartitionedArray<Element, rank>&& array) -> api::Field
    {
        return {std::move(array)};
    }


    template<Arithmetic Element>
    auto as_field(Element const element) -> api::Field
    {
        return {element};
    }


    template<Arithmetic Element>
    auto as_field(Scalar<Element>&& scalar) -> api::Field
    {
        return {std::move(scalar)};
    }


    namespace api {

        // as_field grabs the internals of the variant passed in. This variant isn't useful anymore
        // afterwards.

        auto as_field(Array array) -> Field
        {
            return std::visit(
                overload{
                    [](auto&& array) -> Field
                    { return lue::as_field(std::forward<decltype(array)>(array)); }},
                std::move(array.variant()));
        }


        auto as_field(Literal literal) -> Field
        {
            return std::visit(
                overload{[](auto literal) -> Field { return lue::as_field(literal); }}, literal.variant());
        }


        auto as_field(Scalar scalar) -> Field
        {
            return std::visit(
                overload{
                    [](auto&& scalar) -> Field
                    { return lue::as_field(std::forward<decltype(scalar)>(scalar)); }},
                std::move(scalar.variant()));
        }

    }  // namespace api
}  // namespace lue

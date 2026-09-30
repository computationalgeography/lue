#include "lue/framework/api/cxx/type_info.hpp"
#include "lue/framework/api/cxx/detail/unsupported_overload.hpp"
#include "lue/overload.hpp"
#include "lue/framework/core/type_traits.hpp"


namespace lue {

    auto data_model(auto const& field) -> DataModel
    {
        api::detail::unsupported_overload("data_model", field);

        return {};
    }


    template<Arithmetic Element, Rank rank>
    auto data_model([[maybe_unused]] PartitionedArray<Element, rank> const& array)
    {
        return DataModel::Array;
    }


    // auto element_type(auto const& field) -> ElementType
    // {
    //     api::detail::unsupported_overload("element_type", field);
    //
    //     return {};
    // }


    template<Arithmetic Element>
    auto element_type([[maybe_unused]] PartitionedRaster<Element> const& raster)
    {
        return TypeTraits<Element>::element_type;
    }


    template<Arithmetic Element, Rank rank>
    auto element_type([[maybe_unused]] PartitionedArray<Element, rank> const& array)
    {
        return TypeTraits<Element>::element_type;
    }


    template<Arithmetic Element>
    auto element_type([[maybe_unused]] Scalar<Element> const& scalar)
    {
        return TypeTraits<Element>::element_type;
    }


    template<Arithmetic Element>
    auto element_type([[maybe_unused]] Element const element)
    {
        return TypeTraits<Element>::element_type;
    }


    namespace api {

        auto data_model(Field const& field) -> DataModel
        {
            return std::visit(
                overload{[](auto const& field) -> DataModel { return lue::data_model(field); }},
                field.variant());
        }


        auto element_type(Array const& array) -> ElementType
        {
            return std::visit(
                overload{[](auto const& array) -> ElementType { return lue::element_type(array); }},
                array.variant());
        }


        auto element_type(Field const& field) -> ElementType
        {
            return std::visit(
                overload{[](auto const& field) -> ElementType { return lue::element_type(field); }},
                field.variant());
        }


        auto element_type(Literal const& literal) -> ElementType
        {
            return std::visit(
                overload{[](auto const& literal) -> ElementType { return lue::element_type(literal); }},
                literal.variant());
        }


        auto element_type(Scalar const& scalar) -> ElementType
        {
            return std::visit(
                overload{[](auto const& scalar) -> ElementType { return lue::element_type(scalar); }},
                scalar.variant());
        }

    }  // namespace api
}  // namespace lue

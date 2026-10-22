#include "type_info.hpp"
#include "array.hpp"
#include "field.hpp"
#include "literal.hpp"
#include "scalar.hpp"


auto lue_data_model(LUE_Field const* field) -> LUE_DataModel
{
    auto scoped_enum = lue::api::data_model(lue_as_cxx_field(field));
    LUE_DataModel result{};

    switch (scoped_enum)
    {
        case lue::DataModel::Array:
        {
            result = LUE_DataModel_Array;
            break;
        }
        case lue::DataModel::Literal:
        {
            result = LUE_DataModel_Literal;
            break;
        }
        case lue::DataModel::Scalar:
        {
            result = LUE_DataModel_Scalar;
            break;
        }
    }

    return result;
}


static auto element_type(lue::ElementType const scoped_enum)
{
    return lue_from_cxx_enum(scoped_enum);
}


auto lue_element_type_array(LUE_Array const* array) -> LUE_ElementType
{
    auto scoped_enum = lue::api::element_type(lue_as_cxx_array(array));

    return element_type(scoped_enum);
}


auto lue_element_type_field(LUE_Field const* field) -> LUE_ElementType
{
    auto scoped_enum = lue::api::element_type(lue_as_cxx_field(field));

    return element_type(scoped_enum);
}


auto lue_element_type_literal(LUE_Literal const* literal) -> LUE_ElementType
{
    auto scoped_enum = lue::api::element_type(lue_as_cxx_literal(literal));

    return element_type(scoped_enum);
}


auto lue_element_type_scalar(LUE_Scalar const* scalar) -> LUE_ElementType
{
    auto scoped_enum = lue::api::element_type(lue_as_cxx_scalar(scalar));

    return element_type(scoped_enum);
}

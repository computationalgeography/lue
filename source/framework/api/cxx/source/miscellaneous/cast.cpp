#include "lue/framework/algorithm/value_policies/cast.hpp"
#include "lue/framework/api/cxx/detail/unsupported_overload.hpp"
#include "lue/framework/api/cxx/miscellaneous/cast.hpp"
#include "lue/overload.hpp"


namespace lue::api {

    namespace value_policies {

        template<typename OutputElement>
        auto cast(auto const& field) -> api::Field
        {
            api::detail::unsupported_overload("cast", field);

            return {};
        }

    }  // namespace value_policies


#define HANDLE_CASE(enum_, type)                                                                             \
    case ElementType::enum_:                                                                                 \
    {                                                                                                        \
        return value_policies::cast<type>(field);                                                            \
    }


    auto cast(Field const& field, ElementType const element_type) -> Field
    {
        return std::visit(
            overload{
                [element_type](auto const& field) -> Field
                {
                    switch (element_type)
                    {
#ifdef LUE_FRAMEWORK_WITH_INT8_ELEMENT
                        HANDLE_CASE(Int8, std::int8_t)
#endif
#ifdef LUE_FRAMEWORK_WITH_INT16_ELEMENT
                        HANDLE_CASE(Int16, std::int16_t)
#endif
#ifdef LUE_FRAMEWORK_WITH_INT32_ELEMENT
                        HANDLE_CASE(Int32, std::int32_t)
#endif
#ifdef LUE_FRAMEWORK_WITH_INT64_ELEMENT
                        HANDLE_CASE(Int64, std::int64_t)
#endif
#ifdef LUE_FRAMEWORK_WITH_UINT8_ELEMENT
                        HANDLE_CASE(UInt8, std::uint8_t)
#endif
#ifdef LUE_FRAMEWORK_WITH_UINT16_ELEMENT
                        HANDLE_CASE(UInt16, std::uint16_t)
#endif
#ifdef LUE_FRAMEWORK_WITH_UINT32_ELEMENT
                        HANDLE_CASE(UInt32, std::uint32_t)
#endif
#ifdef LUE_FRAMEWORK_WITH_UINT64_ELEMENT
                        HANDLE_CASE(UInt64, std::uint64_t)
#endif
#ifdef LUE_FRAMEWORK_WITH_FLOAT32_ELEMENT
                        HANDLE_CASE(Float32, float)
#endif
#ifdef LUE_FRAMEWORK_WITH_FLOAT64_ELEMENT
                        HANDLE_CASE(Float64, double)
#endif
                        default:
                        {
                            api::detail::unsupported_overload("field", field);

                            return {};
                        }
                    }
                }},
            field.variant());
    }


#undef HANDLE_CASE

}  // namespace lue::api

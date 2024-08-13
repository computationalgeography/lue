#pragma once
#include "lue/gdal/configure.hpp"
#include <gdal_priv.h>


namespace lue::gdal {

    /*!
        @brief      Type-dependant information
        @tparam     Element Type of individual elements in a raster

        Members:
        - @a data_type: GDAL type-id corresponding with the @a Element type
    */
    template<typename Element>
    class TypeTraits
    {
    };


    template<>
    class TypeTraits<std::uint8_t>
    {

        public:

            static constexpr GDALDataType data_type{GDT_Byte};
    };


#if LUE_GDAL_SUPPORTS_8BIT_SIGNED_INTEGERS
    template<>
    class TypeTraits<std::int8_t>
    {

        public:

            static constexpr GDALDataType data_type{GDT_Int8};
    };
#endif


    template<>
    class TypeTraits<std::uint16_t>
    {

        public:

            static constexpr GDALDataType data_type{GDT_UInt16};
    };


    template<>
    class TypeTraits<std::int16_t>
    {

        public:

            static constexpr GDALDataType data_type{GDT_Int16};
    };


    template<>
    class TypeTraits<std::uint32_t>
    {

        public:

            static constexpr GDALDataType data_type{GDT_UInt32};
    };


    template<>
    class TypeTraits<std::int32_t>
    {

        public:

            static constexpr GDALDataType data_type{GDT_Int32};
    };


#if LUE_GDAL_SUPPORTS_64BIT_INTEGERS
    template<>
    class TypeTraits<std::uint64_t>
    {

        public:

            static constexpr GDALDataType data_type{GDT_UInt64};
    };


    template<>
    class TypeTraits<std::int64_t>
    {

        public:

            static constexpr GDALDataType data_type{GDT_Int64};
    };
#endif


    template<>
    class TypeTraits<float>
    {

        public:

            static constexpr GDALDataType data_type{GDT_Float32};
    };


    template<>
    class TypeTraits<double>
    {

        public:

            static constexpr GDALDataType data_type{GDT_Float64};
    };


    template<typename Element>
    constexpr GDALDataType data_type_v = TypeTraits<Element>::data_type;

}  // namespace lue::gdal

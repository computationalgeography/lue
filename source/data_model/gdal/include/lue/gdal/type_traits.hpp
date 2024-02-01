#pragma once
#include <gdal_priv.h>


namespace lue::gdal {

    /*!
        @brief      Type-dependant information
        @tparam     Element Type of individual elements in a raster

        Members:
        - @a type_id: GDAL type-id corresponding with @a Element
    */
    template<typename Element>
    class TypeTraits
    {
    };


    template<>
    class TypeTraits<std::uint8_t>
    {

        public:

            static constexpr GDALDataType type_id{GDT_Byte};
    };


    template<>
    class TypeTraits<std::uint16_t>
    {

        public:

            static constexpr GDALDataType type_id{GDT_UInt16};
    };


    template<>
    class TypeTraits<std::int16_t>
    {

        public:

            static constexpr GDALDataType type_id{GDT_Int16};
    };


    template<>
    class TypeTraits<std::uint32_t>
    {

        public:

            static constexpr GDALDataType type_id{GDT_UInt32};
    };


    template<>
    class TypeTraits<std::int32_t>
    {

        public:

            static constexpr GDALDataType type_id{GDT_Int32};
    };


#if GDAL_VERSION_NUM >= GDAL_COMPUTE_VERSION(3, 5, 0)
    template<>
    class TypeTraits<std::uint64_t>
    {

        public:

            static constexpr GDALDataType type_id{GDT_UInt64};
    };


    template<>
    class TypeTraits<std::int64_t>
    {

        public:

            static constexpr GDALDataType type_id{GDT_Int64};
    };
#endif


    template<>
    class TypeTraits<float>
    {

        public:

            static constexpr GDALDataType type_id{GDT_Float32};
    };


    template<>
    class TypeTraits<double>
    {

        public:

            static constexpr GDALDataType type_id{GDT_Float64};
    };

}  // namespace lue::gdal

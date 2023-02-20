#pragma once
#include <gdal_priv.h>


namespace lue {

    template<typename Element>
    class GDALTypeTraits
    {
    };


    template<>
    class GDALTypeTraits<std::uint8_t>
    {

        public:

            static constexpr GDALDataType type_id{GDT_Byte};
    };


    template<>
    class GDALTypeTraits<std::uint16_t>
    {

        public:

            static constexpr GDALDataType type_id{GDT_UInt16};
    };


    template<>
    class GDALTypeTraits<std::int16_t>
    {

        public:

            static constexpr GDALDataType type_id{GDT_Int16};
    };


    template<>
    class GDALTypeTraits<std::uint32_t>
    {

        public:

            static constexpr GDALDataType type_id{GDT_UInt32};
    };


    template<>
    class GDALTypeTraits<std::int32_t>
    {

        public:

            static constexpr GDALDataType type_id{GDT_Int32};
    };


#if GDAL_VERSION_NUM >= GDAL_COMPUTE_VERSION(3, 5, 0)
    template<>
    class GDALTypeTraits<std::uint64_t>
    {

        public:

            static constexpr GDALDataType type_id{GDT_UInt64};
    };


    template<>
    class GDALTypeTraits<std::int64_t>
    {

        public:

            static constexpr GDALDataType type_id{GDT_Int64};
    };
#endif


    template<>
    class GDALTypeTraits<float>
    {

        public:

            static constexpr GDALDataType type_id{GDT_Float32};
    };


    template<>
    class GDALTypeTraits<double>
    {

        public:

            static constexpr GDALDataType type_id{GDT_Float64};
    };

}  // namespace lue

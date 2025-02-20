#pragma once
#include "lue/gdal/configure.hpp"
#include "lue/gdal/export.hpp"
#include <stdexcept>
#include <type_traits>


namespace lue::gdal {

    /*!
        @brief      Verify whether GDAL supports I/O of @a Element values
        @tparam     Element Type of values
        @exception  std::runtime_error In case GDAL does not support I/O of @a Element values
    */
    template<typename Element>
    void LUE_GDAL_EXPORT verify_support()
    {
        if constexpr (
            (std::is_same_v<Element, std::int64_t> || std::is_same_v<Element, std::uint64_t>) &&
            !supports_64bit_integers)
        {
            throw std::runtime_error("The GDAL library doesn't support I/O of 64-bit integrals. "
                                     "Upgrade GDAL to version ≥ 3.5.0.");
        }

        if constexpr (std::is_same_v<Element, std::int8_t> && !supports_8bit_signed_integers)
        {
            throw std::runtime_error("The GDAL library doesn't support I/O of signed 8-bit integrals. "
                                     "Upgrade GDAL to version ≥ 3.7.0.");
        }
    }


    template<typename Element>
    constexpr LUE_GDAL_EXPORT auto supports() -> bool
    {
        if constexpr (std::is_same_v<Element, std::int8_t>)
        {
            return supports_8bit_signed_integers;
        }

        if constexpr (std::is_same_v<Element, std::int64_t> || std::is_same_v<Element, std::uint64_t>)
        {
            return supports_64bit_integers;
        }

        return true;
    }

}  // namespace lue::gdal

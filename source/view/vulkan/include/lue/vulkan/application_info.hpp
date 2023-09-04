#pragma once
#include <vulkan/vulkan.h>
#include <cstdint>
#include <string>
#include <tuple>
#include <vector>


namespace lue::vulkan {

    using Names = std::vector<char const*>;


    /*!
        @brief      .
        @param      .
        @return     .
        @exception  .
        @sa         https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkApplicationInfo.html
    */
    class ApplicationInfo
    {

        public:

            ApplicationInfo(
                std::string const& application_name,
                std::tuple<std::uint32_t, std::uint32_t, std::uint32_t> const& version,
                std::uint32_t const api_version);

            ApplicationInfo(ApplicationInfo const&) = delete;

            ApplicationInfo(ApplicationInfo&&) = default;

            ~ApplicationInfo() = default;

            ApplicationInfo& operator=(ApplicationInfo const&) = delete;

            ApplicationInfo& operator=(ApplicationInfo&&) = default;

            operator VkApplicationInfo const*() const;

        private:

            std::string _application_name;

            std::string _engine_name;

            VkApplicationInfo _application_info;
    };

}  // namespace lue::vulkan

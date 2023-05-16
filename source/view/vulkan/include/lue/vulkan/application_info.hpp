#pragma once
#include <string>
#include <vulkan/vulkan.h>


namespace lue::vulkan {

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

            ApplicationInfo(std::string const& application_name, std::string const& engine_name);

            ApplicationInfo(ApplicationInfo const&) = delete;

            ApplicationInfo(ApplicationInfo&&) = default;

            ~ApplicationInfo();

            ApplicationInfo& operator=(ApplicationInfo const&) = delete;

            ApplicationInfo& operator=(ApplicationInfo&&) = default;

            operator VkApplicationInfo const*();

        private:

            VkApplicationInfo _application_info;
    };

}  // namespace lue::vulkan

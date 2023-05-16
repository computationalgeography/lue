#pragma once
#include "lue/vulkan/application_info.hpp"
#include <vector>


namespace lue::vulkan {

    /*!
        @brief      .
        @param      .
        @return     .
        @exception  .
        @sa https://registry.khronos.org/vulkan/specs/1.3-extensions/man/html/VkInstanceCreateInfo.html
    */
    class InstanceCreateInfo
    {

        public:

            using Names = std::vector<char const*>;

            InstanceCreateInfo(
                VkInstanceCreateFlags const flags,
                ApplicationInfo&& application_info,
                Names const& enabled_layer_names,
                Names const& enabled_extension_names);

            InstanceCreateInfo(
                ApplicationInfo&& application_info,
                Names const& enabled_layer_names,
                Names const& enabled_extension_names);

            InstanceCreateInfo(ApplicationInfo&& application_info);

            InstanceCreateInfo(InstanceCreateInfo const&) = delete;

            InstanceCreateInfo(InstanceCreateInfo&&) = default;

            ~InstanceCreateInfo();

            InstanceCreateInfo& operator=(InstanceCreateInfo const&) = delete;

            InstanceCreateInfo& operator=(InstanceCreateInfo&&) = default;

            operator VkInstanceCreateInfo const*();

        private:

            ApplicationInfo _application_info;

            VkInstanceCreateInfo _instance_create_info;
    };

}  // namespace lue::vulkan

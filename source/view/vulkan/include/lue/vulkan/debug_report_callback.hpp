#pragma once
#include "lue/vulkan/instance.hpp"


namespace lue::vulkan {

    class DebugReportCallback
    {

        public:

            class CreateInfo
            {

                public:

                    CreateInfo(VkDebugReportFlagsEXT const flags, PFN_vkDebugReportCallbackEXT callback);

                    CreateInfo(CreateInfo const&) = delete;

                    CreateInfo(CreateInfo&&) = default;

                    ~CreateInfo() = default;

                    CreateInfo& operator=(CreateInfo const&) = delete;

                    CreateInfo& operator=(CreateInfo&&) = default;

                    operator VkDebugReportCallbackCreateInfoEXT const*() const;

                private:

                    VkDebugReportCallbackCreateInfoEXT _create_info;
            };

            DebugReportCallback(Instance const& instance, CreateInfo&& create_info);

            DebugReportCallback(DebugReportCallback const&) = delete;

            DebugReportCallback(DebugReportCallback&&) = default;

            ~DebugReportCallback();

            DebugReportCallback& operator=(DebugReportCallback const&) = delete;

            DebugReportCallback& operator=(DebugReportCallback&&) = default;

        private:

            VkInstance const* _instance;

            VkDebugReportCallbackEXT _callback;

            PFN_vkDestroyDebugReportCallbackEXT _destroy_callback;
    };

}  // namespace lue::vulkan

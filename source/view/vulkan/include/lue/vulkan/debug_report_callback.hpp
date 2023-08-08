#pragma once
#include "lue/vulkan/instance.hpp"


namespace lue::vulkan {

    // TODO What is the difference between VkDebugUtilsMessengerCreateInfoExt etc and the stuff
    //      used here?

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

                    static_assert(!std::is_pointer_v<VkDebugReportCallbackCreateInfoEXT>);

                    VkDebugReportCallbackCreateInfoEXT _create_info;
            };

            DebugReportCallback();

            DebugReportCallback(Instance const& instance, CreateInfo&& create_info);

            DebugReportCallback(DebugReportCallback const&) = delete;

            DebugReportCallback(DebugReportCallback&& other);

            ~DebugReportCallback();

            DebugReportCallback& operator=(DebugReportCallback const&) = delete;

            DebugReportCallback& operator=(DebugReportCallback&& other);

            operator bool() const;

        private:

            //! Function to create callback instances
            static PFN_vkCreateDebugReportCallbackEXT _create_callback;

            //! Function to destroy the callback instance
            static PFN_vkDestroyDebugReportCallbackEXT _destroy_callback;

            static_assert(std::is_pointer_v<VkInstance>);

            VkInstance _instance;

            static_assert(std::is_pointer_v<VkDebugReportCallbackEXT>);

            VkDebugReportCallbackEXT _callback;
    };

}  // namespace lue::vulkan

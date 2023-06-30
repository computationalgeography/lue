#pragma once
#include <vulkan/vulkan.h>
#include <type_traits>


namespace lue::vulkan {

    class Swapchain
    {

            class CreateInfo
            {

                public:

                    CreateInfo() = delete;

                    CreateInfo(CreateInfo const&) = delete;

                    CreateInfo(CreateInfo&&) = default;

                    ~CreateInfo() = default;

                    CreateInfo& operator=(CreateInfo const&) = delete;

                    CreateInfo& operator=(CreateInfo&&) = default;

                    operator VkSwapchainCreateInfoKHR const*() const;

                private:

                    static_assert(!std::is_pointer_v<VkSwapchainCreateInfoKHR>);

                    VkSwapchainCreateInfoKHR _create_info;
            };


        public:

        private:
    };

}  // namespace lue::vulkan

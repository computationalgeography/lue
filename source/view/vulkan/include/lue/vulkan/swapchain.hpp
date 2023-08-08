#pragma once
#include "lue/vulkan/queue_family.hpp"
#include "lue/vulkan/surface.hpp"
#include <cstdint>


namespace lue::vulkan {

    /*!
        @brief      A queue of images waiting to be presented to the screen

        General purpose of the swapchain is to synchronize the presentation of images with the
        refresh rate of the screen.
    */
    class Swapchain
    {

        public:

            class CreateInfo
            {

                public:

                    CreateInfo(
                        Surface const& surface,
                        std::uint32_t const min_nr_images,
                        VkSurfaceFormatKHR const image_format,
                        VkExtent2D const image_extent,
                        QueueFamilies const& queue_families,
                        VkSurfaceTransformFlagBitsKHR const transform,
                        VkPresentModeKHR const present_mode);

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


            using Images = std::vector<VkImage>;


            Swapchain();

            Swapchain(VkDevice device, VkSwapchainKHR swapchain);

            Swapchain(Swapchain const&) = delete;

            Swapchain(Swapchain&& other);

            ~Swapchain();

            Swapchain& operator=(Swapchain const&) = delete;

            Swapchain& operator=(Swapchain&& other);

            operator bool() const;

            operator VkSwapchainKHR() const;

            Images images() const;

        private:

            static_assert(std::is_pointer_v<VkDevice>);

            VkDevice _device;

            static_assert(std::is_pointer_v<VkSwapchainKHR>);

            VkSwapchainKHR _swapchain;
    };

}  // namespace lue::vulkan

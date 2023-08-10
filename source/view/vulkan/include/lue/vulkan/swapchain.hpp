#pragma once
#include "lue/vulkan/structure.hpp"
#include <cstdint>
#include <vector>


namespace lue::vulkan {

    /*!
        @brief      A queue of images waiting to be presented to the screen

        General purpose of the swapchain is to synchronize the presentation of images with the
        refresh rate of the screen.
    */
    class Swapchain
    {

        public:

            using CreateInfo =
                Structure<VkSwapchainCreateInfoKHR, VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR>;

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

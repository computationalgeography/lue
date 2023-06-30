#include "lue/vulkan/swapchain.hpp"


namespace lue::vulkan {

    // Swapchain::CreateInfo::CreateInfo():

    //     _create_info{}

    // {
    //     _create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    //     // hier verder
    // }


    /*!
        @warning    Do not use the returned pointer after this instance has gone out of scope
    */
    Swapchain::CreateInfo::operator VkSwapchainCreateInfoKHR const*() const
    {
        return &_create_info;
    }

}  // namespace lue::vulkan

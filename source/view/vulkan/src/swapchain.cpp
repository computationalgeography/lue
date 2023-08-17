#include "lue/vulkan/swapchain.hpp"
#include <cassert>
#include <utility>


namespace lue::vulkan {

    Swapchain::Swapchain():

        _device{},
        _swapchain{}

    {
        assert(!is_valid());
    }


    Swapchain::Swapchain(VkDevice device, VkSwapchainKHR swapchain):

        _device{device},
        _swapchain{swapchain}

    {
        assert(is_valid());
    }


    Swapchain::Swapchain(Swapchain&& other):

        _device{std::move(other._device)},
        _swapchain{std::move(other._swapchain)}

    {
        other._device = VkDevice{};
        other._swapchain = VkSwapchainKHR{};

        assert(!other.is_valid());
    }


    Swapchain::~Swapchain()
    {
        if (is_valid())
        {
            vkDestroySwapchainKHR(_device, _swapchain, nullptr);

            _device = VkDevice{};
            _swapchain = VkSwapchainKHR{};
        }

        assert(!is_valid());
    }


    Swapchain& Swapchain::operator=(Swapchain&& other)
    {
        if (is_valid())
        {
            vkDestroySwapchainKHR(_device, _swapchain, nullptr);
        }

        _device = std::move(other._device);
        _swapchain = std::move(other._swapchain);

        other._device = VkDevice{};
        other._swapchain = VkSwapchainKHR{};

        assert(!other.is_valid());

        return *this;
    }


    bool Swapchain::is_valid() const
    {
        return _swapchain != VK_NULL_HANDLE;
    }


    Swapchain::operator VkSwapchainKHR()
    {
        assert(is_valid());

        return _swapchain;
    }


    Swapchain::Images Swapchain::images() const
    {
        // Obtain the number of images in the swapchain
        std::uint32_t nr_images;
        vkGetSwapchainImagesKHR(_device, _swapchain, &nr_images, nullptr);

        // Obtain the images themselves
        Images images(nr_images);
        vkGetSwapchainImagesKHR(_device, _swapchain, &nr_images, images.data());

        return images;
    }

}  // namespace lue::vulkan

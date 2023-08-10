#include "lue/vulkan/swapchain.hpp"
#include <cassert>
#include <utility>


namespace lue::vulkan {

    Swapchain::Swapchain():

        _device{},
        _swapchain{}

    {
        assert(!*this);
    }


    Swapchain::Swapchain(VkDevice device, VkSwapchainKHR swapchain):

        _device{device},
        _swapchain{swapchain}

    {
        assert(*this);
    }


    Swapchain::Swapchain(Swapchain&& other):

        _device{std::move(other._device)},
        _swapchain{std::move(other._swapchain)}

    {
        other._device = VkDevice{};
        other._swapchain = VkSwapchainKHR{};

        assert(!other);
    }


    Swapchain::~Swapchain()
    {
        if (*this)
        {
            vkDestroySwapchainKHR(_device, _swapchain, nullptr);
            _device = VkDevice{};
            _swapchain = VkSwapchainKHR{};
        }

        assert(!*this);
    }


    Swapchain& Swapchain::operator=(Swapchain&& other)
    {
        if (*this)
        {
            vkDestroySwapchainKHR(_device, _swapchain, nullptr);
        }

        _device = std::move(other._device);
        _swapchain = std::move(other._swapchain);

        other._device = VkDevice{};
        other._swapchain = VkSwapchainKHR{};

        assert(!other);

        return *this;
    }


    Swapchain::operator bool() const
    {
        return _swapchain != VK_NULL_HANDLE;
    }


    Swapchain::operator VkSwapchainKHR() const
    {
        assert(*this);

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

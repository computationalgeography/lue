#include "lue/vulkan/swapchain.hpp"
#include <cassert>
#include <utility>


namespace lue::vulkan {

    Swapchain::CreateInfo::CreateInfo(
        Surface const& surface,
        std::uint32_t const min_nr_images,
        VkSurfaceFormatKHR const image_format,
        VkExtent2D const image_extent,
        QueueFamilies const& queue_families,
        VkSurfaceTransformFlagBitsKHR const transform,
        VkPresentModeKHR const present_mode):

        _create_info{}

    {
        _create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        _create_info.surface = surface;
        _create_info.minImageCount = min_nr_images;
        _create_info.imageFormat = image_format.format;
        _create_info.imageColorSpace = image_format.colorSpace;
        _create_info.imageExtent = image_extent;

        _create_info.imageArrayLayers = 1;

        _create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        assert(queue_families.is_complete());
        std::uint32_t queue_family_indices[] = {
            queue_families.graphics_family(), queue_families.present_family()};

        if (queue_families.graphics_family() != queue_families.present_family())
        {
            _create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            _create_info.queueFamilyIndexCount = 2;
            _create_info.pQueueFamilyIndices = queue_family_indices;
        }
        else
        {
            _create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            _create_info.queueFamilyIndexCount = 0;      // Optional
            _create_info.pQueueFamilyIndices = nullptr;  // Optional
        }

        _create_info.preTransform = transform;
        _create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

        _create_info.presentMode = present_mode;
        _create_info.clipped = VK_TRUE;

        _create_info.oldSwapchain = VK_NULL_HANDLE;
    }


    /*!
        @warning    Do not use the returned pointer after this instance has gone out of scope
    */
    Swapchain::CreateInfo::operator VkSwapchainCreateInfoKHR const*() const
    {
        return &_create_info;
    }


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

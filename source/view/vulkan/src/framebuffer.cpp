#include "lue/vulkan/framebuffer.hpp"
#include <cassert>
#include <utility>


namespace lue::vulkan {

    Framebuffer::Framebuffer():

        _device{},
        _framebuffer{}

    {
        assert(!is_valid());
    }


    Framebuffer::Framebuffer(VkDevice device, VkFramebuffer framebuffer):

        _device{device},
        _framebuffer{framebuffer}

    {
        assert(is_valid());
    }


    Framebuffer::Framebuffer(Framebuffer&& other):

        _device{std::move(other._device)},
        _framebuffer{std::move(other._framebuffer)}

    {
        other._device = VkDevice{};
        other._framebuffer = VkFramebuffer{};

        assert(!other.is_valid());
    }


    Framebuffer::~Framebuffer()
    {
        if (is_valid())
        {
            vkDestroyFramebuffer(_device, _framebuffer, nullptr);

            _device = VkDevice{};
            _framebuffer = VkFramebuffer{};
        }

        assert(!is_valid());
    }


    Framebuffer& Framebuffer::operator=(Framebuffer&& other)
    {
        if (is_valid())
        {
            vkDestroyFramebuffer(_device, _framebuffer, nullptr);
        }

        _device = std::move(other._device);
        _framebuffer = std::move(other._framebuffer);

        other._device = VkDevice{};
        other._framebuffer = VkFramebuffer{};

        assert(!other.is_valid());

        return *this;
    }


    bool Framebuffer::is_valid() const
    {
        return _framebuffer != VK_NULL_HANDLE;
    }


    Framebuffer::operator VkFramebuffer()
    {
        assert(is_valid());

        return _framebuffer;
    }

}  // namespace lue::vulkan

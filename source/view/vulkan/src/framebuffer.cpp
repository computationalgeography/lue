#include "lue/vulkan/framebuffer.hpp"
#include <cassert>
#include <utility>


namespace lue::vulkan {

    Framebuffer::Framebuffer():

        _device{},
        _framebuffer{}

    {
        assert(!*this);
    }


    Framebuffer::Framebuffer(VkDevice device, VkFramebuffer framebuffer):

        _device{device},
        _framebuffer{framebuffer}

    {
        assert(*this);
    }


    Framebuffer::Framebuffer(Framebuffer&& other):

        _device{std::move(other._device)},
        _framebuffer{std::move(other._framebuffer)}

    {
        other._device = VkDevice{};
        other._framebuffer = VkFramebuffer{};

        assert(!other);
    }


    Framebuffer::~Framebuffer()
    {
        if (*this)
        {
            vkDestroyFramebuffer(_device, _framebuffer, nullptr);
            _device = VkDevice{};
            _framebuffer = VkFramebuffer{};
        }

        assert(!*this);
    }


    Framebuffer& Framebuffer::operator=(Framebuffer&& other)
    {
        if (*this)
        {
            vkDestroyFramebuffer(_device, _framebuffer, nullptr);
        }

        _device = std::move(other._device);
        _framebuffer = std::move(other._framebuffer);

        other._device = VkDevice{};
        other._framebuffer = VkFramebuffer{};

        assert(!other);

        return *this;
    }


    Framebuffer::operator bool() const
    {
        return _framebuffer != VK_NULL_HANDLE;
    }


    Framebuffer::operator VkFramebuffer() const
    {
        assert(*this);

        return _framebuffer;
    }

}  // namespace lue::vulkan

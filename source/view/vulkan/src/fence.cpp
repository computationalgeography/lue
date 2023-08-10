#include "lue/vulkan/fence.hpp"
#include <cassert>
#include <utility>


namespace lue::vulkan {

    Fence::Fence():

        _device{},
        _fence{}

    {
        assert(!*this);
    }


    Fence::Fence(VkDevice device, VkFence render_pass):

        _device{device},
        _fence{render_pass}

    {
        assert(*this);
    }


    Fence::Fence(Fence&& other):

        _device{std::move(other._device)},
        _fence{std::move(other._fence)}

    {
        other._device = VkDevice{};
        other._fence = VkFence{};

        assert(!other);
    }


    Fence::~Fence()
    {
        if (*this)
        {
            vkDestroyFence(_device, _fence, nullptr);
            _device = VkDevice{};
            _fence = VkFence{};
        }

        assert(!*this);
    }


    Fence& Fence::operator=(Fence&& other)
    {
        if (*this)
        {
            vkDestroyFence(_device, _fence, nullptr);
        }

        _device = std::move(other._device);
        _fence = std::move(other._fence);

        other._device = VkDevice{};
        other._fence = VkFence{};

        assert(!other);

        return *this;
    }


    Fence::operator bool() const
    {
        return _fence != VK_NULL_HANDLE;
    }


    Fence::operator VkFence() const
    {
        assert(*this);

        return _fence;
    }


    Fence::operator VkFence const*() const
    {
        assert(*this);

        return &_fence;
    }

}  // namespace lue::vulkan

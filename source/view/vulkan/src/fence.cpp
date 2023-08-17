#include "lue/vulkan/fence.hpp"
#include <cassert>
#include <utility>


namespace lue::vulkan {

    Fence::Fence():

        _device{},
        _fence{}

    {
        assert(!is_valid());
    }


    Fence::Fence(VkDevice device, VkFence fence):

        _device{device},
        _fence{fence}

    {
        assert(is_valid());
    }


    Fence::Fence(Fence&& other):

        _device{std::move(other._device)},
        _fence{std::move(other._fence)}

    {
        other._device = VkDevice{};
        other._fence = VkFence{};

        assert(!other.is_valid());
    }


    Fence::~Fence()
    {
        if (is_valid())
        {
            vkDestroyFence(_device, _fence, nullptr);

            _device = VkDevice{};
            _fence = VkFence{};
        }

        assert(!is_valid());
    }


    Fence& Fence::operator=(Fence&& other)
    {
        if (is_valid())
        {
            vkDestroyFence(_device, _fence, nullptr);
        }

        _device = std::move(other._device);
        _fence = std::move(other._fence);

        other._device = VkDevice{};
        other._fence = VkFence{};

        assert(!other.is_valid());

        return *this;
    }


    bool Fence::is_valid() const
    {
        return _fence != VK_NULL_HANDLE;
    }


    Fence::operator VkFence()
    {
        assert(is_valid());

        return _fence;
    }


    Fence::operator VkFence const*() const
    {
        assert(is_valid());

        return &_fence;
    }

}  // namespace lue::vulkan

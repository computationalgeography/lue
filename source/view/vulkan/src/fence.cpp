#include "lue/vulkan/fence.hpp"
#include <cassert>
#include <utility>


namespace lue::vulkan {

    Fence::CreateInfo::CreateInfo():

        _create_info{}

    {
        _create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    }


    /*!
        @warning    Do not use the returned pointer after this instance has gone out of scope
    */
    Fence::CreateInfo::operator VkFenceCreateInfo const*() const
    {
        return &_create_info;
    }


    VkFenceCreateInfo& Fence::CreateInfo::operator*()
    {
        return _create_info;
    }


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

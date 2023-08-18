#include "lue/vulkan/descriptor_pool.hpp"
#include <cassert>
#include <utility>


namespace lue::vulkan {

    DescriptorPool::DescriptorPool():

        _device{},
        _pool{}

    {
        assert(!is_valid());
    }


    DescriptorPool::DescriptorPool(VkDevice device, VkDescriptorPool buffer):

        _device{device},
        _pool{buffer}

    {
        assert(is_valid());
    }


    DescriptorPool::DescriptorPool(DescriptorPool&& other):

        _device{std::move(other._device)},
        _pool{std::move(other._pool)}

    {
        other._device = VkDevice{};
        other._pool = VkDescriptorPool{};

        assert(!other.is_valid());
    }


    DescriptorPool::~DescriptorPool()
    {
        if (is_valid())
        {
            vkDestroyDescriptorPool(_device, _pool, nullptr);

            _device = VkDevice{};
            _pool = VkDescriptorPool{};
        }

        assert(!is_valid());
    }


    DescriptorPool& DescriptorPool::operator=(DescriptorPool&& other)
    {
        if (is_valid())
        {
            vkDestroyDescriptorPool(_device, _pool, nullptr);
        }

        _device = std::move(other._device);
        _pool = std::move(other._pool);

        other._device = VkDevice{};
        other._pool = VkDescriptorPool{};

        assert(!other.is_valid());

        return *this;
    }


    bool DescriptorPool::is_valid() const
    {
        return _pool != VK_NULL_HANDLE;
    }


    DescriptorPool::operator VkDescriptorPool()
    {
        assert(is_valid());

        return _pool;
    }


    DescriptorPool::operator VkDescriptorPool const*() const
    {
        assert(is_valid());

        return &_pool;
    }

}  // namespace lue::vulkan

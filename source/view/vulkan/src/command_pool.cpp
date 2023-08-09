#include "lue/vulkan/command_pool.hpp"
#include <cassert>
#include <utility>


namespace lue::vulkan {

    CommandPool::CommandPool():

        _device{},
        _command_pool{}

    {
        assert(!*this);
    }


    CommandPool::CommandPool(VkDevice device, VkCommandPool command_pool):

        _device{device},
        _command_pool{command_pool}

    {
        assert(*this);
    }


    CommandPool::CommandPool(CommandPool&& other):

        _device{std::move(other._device)},
        _command_pool{std::move(other._command_pool)}

    {
        other._device = VkDevice{};
        other._command_pool = VkCommandPool{};

        assert(!other);
    }


    CommandPool::~CommandPool()
    {
        if (*this)
        {
            vkDestroyCommandPool(_device, _command_pool, nullptr);
            _device = VkDevice{};
            _command_pool = VkCommandPool{};
        }

        assert(!*this);
    }


    CommandPool& CommandPool::operator=(CommandPool&& other)
    {
        if (*this)
        {
            vkDestroyCommandPool(_device, _command_pool, nullptr);
        }

        _device = std::move(other._device);
        _command_pool = std::move(other._command_pool);

        other._device = VkDevice{};
        other._command_pool = VkCommandPool{};

        assert(!other);

        return *this;
    }


    CommandPool::operator bool() const
    {
        return _command_pool != VK_NULL_HANDLE;
    }


    CommandPool::operator VkCommandPool() const
    {
        assert(*this);

        return _command_pool;
    }

}  // namespace lue::vulkan

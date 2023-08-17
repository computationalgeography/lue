#include "lue/vulkan/command_pool.hpp"
#include <cassert>
#include <utility>


namespace lue::vulkan {

    CommandPool::CommandPool():

        _device{},
        _command_pool{}

    {
        assert(!is_valid());
    }


    CommandPool::CommandPool(VkDevice device, VkCommandPool command_pool):

        _device{device},
        _command_pool{command_pool}

    {
        assert(is_valid());
    }


    CommandPool::CommandPool(CommandPool&& other):

        _device{std::move(other._device)},
        _command_pool{std::move(other._command_pool)}

    {
        other._device = VkDevice{};
        other._command_pool = VkCommandPool{};

        assert(!other.is_valid());
    }


    CommandPool::~CommandPool()
    {
        if (is_valid())
        {
            vkDestroyCommandPool(_device, _command_pool, nullptr);

            _device = VkDevice{};
            _command_pool = VkCommandPool{};
        }

        assert(!is_valid());
    }


    CommandPool& CommandPool::operator=(CommandPool&& other)
    {
        if (is_valid())
        {
            vkDestroyCommandPool(_device, _command_pool, nullptr);
        }

        _device = std::move(other._device);
        _command_pool = std::move(other._command_pool);

        other._device = VkDevice{};
        other._command_pool = VkCommandPool{};

        assert(!other.is_valid());

        return *this;
    }


    bool CommandPool::is_valid() const
    {
        return _command_pool != VK_NULL_HANDLE;
    }


    CommandPool::operator VkCommandPool()
    {
        assert(is_valid());

        return _command_pool;
    }

}  // namespace lue::vulkan

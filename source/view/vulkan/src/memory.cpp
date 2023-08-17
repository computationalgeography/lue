#include "lue/vulkan/memory.hpp"
#include <cassert>
#include <utility>


namespace lue::vulkan {

    Memory::Memory():

        _device{},
        _memory{}

    {
        assert(!is_valid());
    }


    Memory::Memory(VkDevice device, VkDeviceMemory memory):

        _device{device},
        _memory{memory}

    {
        assert(is_valid());
    }


    Memory::Memory(Memory&& other):

        _device{std::move(other._device)},
        _memory{std::move(other._memory)}

    {
        other._device = VkDevice{};
        other._memory = VkDeviceMemory{};

        assert(!other.is_valid());
    }


    Memory::~Memory()
    {
        if (is_valid())
        {
            vkFreeMemory(_device, _memory, nullptr);

            _device = VkDevice{};
            _memory = VkDeviceMemory{};
        }

        assert(!is_valid());
    }


    Memory& Memory::operator=(Memory&& other)
    {
        if (is_valid())
        {
            vkFreeMemory(_device, _memory, nullptr);
        }

        _device = std::move(other._device);
        _memory = std::move(other._memory);

        other._device = VkDevice{};
        other._memory = VkDeviceMemory{};

        assert(!other.is_valid());

        return *this;
    }


    bool Memory::is_valid() const
    {
        return _memory != VK_NULL_HANDLE;
    }


    Memory::operator VkDeviceMemory()
    {
        assert(is_valid());

        return _memory;
    }


    Memory::operator VkDeviceMemory const*() const
    {
        assert(is_valid());

        return &_memory;
    }

}  // namespace lue::vulkan

#include "lue/vulkan/buffer.hpp"
#include "lue/vulkan/error.hpp"
#include <cassert>
#include <utility>


namespace lue::vulkan {

    // Buffer::MemoryRequirements::MemoryRequirements():

    //     _requirements{}

    // {
    // }


    // Buffer::MemoryRequirements::MemoryRequirements(VkMemoryRequirements&& requirements):

    //     _requirements{std::move(requirements)}

    // {
    // }


    Buffer::Buffer():

        _device{},
        _buffer{}

    {
        assert(!is_valid());
    }


    Buffer::Buffer(VkDevice device, VkBuffer buffer):

        _device{device},
        _buffer{buffer}

    {
        assert(is_valid());
    }


    Buffer::Buffer(Buffer&& other):

        _device{std::move(other._device)},
        _buffer{std::move(other._buffer)}

    {
        other._device = VkDevice{};
        other._buffer = VkBuffer{};

        assert(!other.is_valid());
    }


    Buffer::~Buffer()
    {
        if (is_valid())
        {
            vkDestroyBuffer(_device, _buffer, nullptr);

            _device = VkDevice{};
            _buffer = VkBuffer{};
        }

        assert(!is_valid());
    }


    Buffer& Buffer::operator=(Buffer&& other)
    {
        if (is_valid())
        {
            vkDestroyBuffer(_device, _buffer, nullptr);
        }

        _device = std::move(other._device);
        _buffer = std::move(other._buffer);

        other._device = VkDevice{};
        other._buffer = VkBuffer{};

        assert(!other.is_valid());

        return *this;
    }


    bool Buffer::is_valid() const
    {
        return _buffer != VK_NULL_HANDLE;
    }


    Buffer::operator VkBuffer()
    {
        assert(is_valid());

        return _buffer;
    }


    Buffer::operator VkBuffer const*() const
    {
        assert(is_valid());

        return &_buffer;
    }


    VkMemoryRequirements Buffer::memory_requirements() const
    {
        VkMemoryRequirements requirements;

        vkGetBufferMemoryRequirements(_device, _buffer, &requirements);

        return requirements;
    }


    void Buffer::bind(Memory& memory, VkDeviceSize const memory_offset)
    {
        VkResult result = vkBindBufferMemory(_device, _buffer, memory, memory_offset);

        assert_result_is_ok(result);
    }

}  // namespace lue::vulkan

#include "lue/vulkan/command_buffer.hpp"
#include <cassert>
#include <utility>


namespace lue::vulkan {

    CommandBuffer::AllocateInfo::AllocateInfo():

        _allocate_info{}

    {
        _allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    }


    /*!
        @warning    Do not use the returned pointer after this instance has gone out of scope
    */
    CommandBuffer::AllocateInfo::operator VkCommandBufferAllocateInfo const*() const
    {
        return &_allocate_info;
    }


    VkCommandBufferAllocateInfo& CommandBuffer::AllocateInfo::operator*()
    {
        return _allocate_info;
    }


    CommandBuffer::CommandBuffer():

        _device{},
        _command_buffer{}

    {
        assert(!*this);
    }


    CommandBuffer::CommandBuffer(VkDevice device, VkCommandBuffer command_buffer):

        _device{device},
        _command_buffer{command_buffer}

    {
        assert(*this);
    }


    CommandBuffer::CommandBuffer(CommandBuffer&& other):

        _device{std::move(other._device)},
        _command_buffer{std::move(other._command_buffer)}

    {
        other._device = VkDevice{};
        other._command_buffer = VkCommandBuffer{};

        assert(!other);
    }


    // CommandBuffer::~CommandBuffer()
    // {
    //     if (*this)
    //     {
    //         vkDestroyCommandBuffer(_device, _command_buffer, nullptr);
    //         _device = VkDevice{};
    //         _command_buffer = VkCommandBuffer{};
    //     }

    //     assert(!*this);
    // }


    CommandBuffer& CommandBuffer::operator=(CommandBuffer&& other)
    {
        // if (*this)
        // {
        //     vkDestroyCommandBuffer(_device, _command_buffer, nullptr);
        // }

        _device = std::move(other._device);
        _command_buffer = std::move(other._command_buffer);

        other._device = VkDevice{};
        other._command_buffer = VkCommandBuffer{};

        assert(!other);

        return *this;
    }


    CommandBuffer::operator bool() const
    {
        return _command_buffer != VK_NULL_HANDLE;
    }


    CommandBuffer::operator VkCommandBuffer() const
    {
        assert(*this);

        return _command_buffer;
    }


    CommandBuffer::operator VkCommandBuffer const*() const
    {
        assert(*this);

        return &_command_buffer;
    }

}  // namespace lue::vulkan

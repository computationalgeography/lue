#pragma once
#include <vulkan/vulkan.h>
#include <type_traits>


namespace lue::vulkan {

    class CommandBuffer
    {
        public:

            class AllocateInfo
            {

                public:

                    AllocateInfo();

                    AllocateInfo(AllocateInfo const&) = delete;

                    AllocateInfo(AllocateInfo&&) = default;

                    ~AllocateInfo() = default;

                    AllocateInfo& operator=(AllocateInfo const&) = delete;

                    AllocateInfo& operator=(AllocateInfo&&) = default;

                    operator VkCommandBufferAllocateInfo const*() const;

                    VkCommandBufferAllocateInfo& operator*();

                private:

                    VkCommandBufferAllocateInfo _allocate_info;
            };


            CommandBuffer();

            CommandBuffer(VkDevice device, VkCommandBuffer command_buffer);

            CommandBuffer(CommandBuffer const&) = delete;

            CommandBuffer(CommandBuffer&& other);

            ~CommandBuffer() = default;

            CommandBuffer& operator=(CommandBuffer const&) = delete;

            CommandBuffer& operator=(CommandBuffer&& other);

            operator bool() const;

            operator VkCommandBuffer() const;

            operator VkCommandBuffer const*() const;

        private:

            static_assert(std::is_pointer_v<VkDevice>);

            VkDevice _device;

            static_assert(std::is_pointer_v<VkCommandBuffer>);

            VkCommandBuffer _command_buffer;
    };

}  // namespace lue::vulkan

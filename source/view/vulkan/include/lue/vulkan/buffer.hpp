#pragma once
#include "lue/vulkan/memory.hpp"
#include "lue/vulkan/structure.hpp"
#include <vulkan/vulkan.h>


namespace lue::vulkan {

    class Buffer
    {
        public:

            using CreateInfo = Structure<VkBufferCreateInfo, VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO>;


            // class MemoryRequirements
            // {

            //     public:

            //         MemoryRequirements();

            //         MemoryRequirements(VkMemoryRequirements&& requirements);

            //         MemoryRequirements(MemoryRequirements const&) = default;

            //         MemoryRequirements(MemoryRequirements&&) = default;

            //         ~MemoryRequirements() = default;

            //         MemoryRequirements& operator=(MemoryRequirements const&) = delete;

            //         MemoryRequirements& operator=(MemoryRequirements&&) = default;

            //     private:

            //         static_assert(!std::is_pointer_v<VkMemoryRequirements>);

            //         VkMemoryRequirements _requirements;
            // };


            Buffer();

            Buffer(VkDevice device, VkBuffer buffer);

            Buffer(Buffer const&) = delete;

            Buffer(Buffer&& other);

            ~Buffer();

            Buffer& operator=(Buffer const&) = delete;

            Buffer& operator=(Buffer&& other);

            bool is_valid() const;

            operator VkBuffer();

            operator VkBuffer const*() const;

            VkMemoryRequirements memory_requirements() const;

            void bind(Memory& memory, VkDeviceSize const memory_offset);

        private:

            static_assert(std::is_pointer_v<VkDevice>);

            VkDevice _device;

            static_assert(std::is_pointer_v<VkBuffer>);

            VkBuffer _buffer;
    };

}  // namespace lue::vulkan

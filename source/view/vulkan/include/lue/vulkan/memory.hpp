#pragma once
#include "lue/vulkan/structure.hpp"
#include <vulkan/vulkan.h>


namespace lue::vulkan {

    class Memory
    {
        public:

            using AllocateInfo = Structure<VkMemoryAllocateInfo, VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO>;

            Memory();

            Memory(VkDevice device, VkDeviceMemory memory);

            Memory(Memory const&) = delete;

            Memory(Memory&& other);

            ~Memory();

            Memory& operator=(Memory const&) = delete;

            Memory& operator=(Memory&& other);

            bool is_valid() const;

            operator VkDeviceMemory();

            operator VkDeviceMemory const*() const;

        private:

            static_assert(std::is_pointer_v<VkDevice>);

            VkDevice _device;

            static_assert(std::is_pointer_v<VkDeviceMemory>);

            VkDeviceMemory _memory;
    };

}  // namespace lue::vulkan

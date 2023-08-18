#pragma once
#include "lue/vulkan/structure.hpp"
#include <vulkan/vulkan.h>


namespace lue::vulkan {

    class DescriptorPool
    {
        public:

            using CreateInfo =
                Structure<VkDescriptorPoolCreateInfo, VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO>;

            using PoolSize = VkDescriptorPoolSize;


            DescriptorPool();

            DescriptorPool(VkDevice device, VkDescriptorPool buffer);

            DescriptorPool(DescriptorPool const&) = delete;

            DescriptorPool(DescriptorPool&& other);

            ~DescriptorPool();

            DescriptorPool& operator=(DescriptorPool const&) = delete;

            DescriptorPool& operator=(DescriptorPool&& other);

            bool is_valid() const;

            operator VkDescriptorPool();

            operator VkDescriptorPool const*() const;

        private:

            static_assert(std::is_pointer_v<VkDevice>);

            VkDevice _device;

            static_assert(std::is_pointer_v<VkDescriptorPool>);

            VkDescriptorPool _pool;
    };

}  // namespace lue::vulkan

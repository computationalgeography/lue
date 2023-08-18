#pragma once
#include "lue/vulkan/structure.hpp"
#include <vulkan/vulkan.h>


namespace lue::vulkan {

    class DescriptorSet
    {
        public:

            using AllocateInfo =
                Structure<VkDescriptorSetAllocateInfo, VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO>;


        private:
    };

}  // namespace lue::vulkan

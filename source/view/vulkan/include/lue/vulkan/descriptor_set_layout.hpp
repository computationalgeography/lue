#pragma once
#include "lue/vulkan/structure.hpp"
#include <vulkan/vulkan.h>


namespace lue::vulkan {

    class DescriptorSetLayout
    {
        public:

            using Binding = VkDescriptorSetLayoutBinding;

            using CreateInfo = Structure<
                VkDescriptorSetLayoutCreateInfo,
                VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO>;


            DescriptorSetLayout();

            DescriptorSetLayout(VkDevice device, VkDescriptorSetLayout layout);

            DescriptorSetLayout(DescriptorSetLayout const&) = delete;

            DescriptorSetLayout(DescriptorSetLayout&& other);

            ~DescriptorSetLayout();

            DescriptorSetLayout& operator=(DescriptorSetLayout const&) = delete;

            DescriptorSetLayout& operator=(DescriptorSetLayout&& other);

            bool is_valid() const;

            operator VkDescriptorSetLayout();

            operator VkDescriptorSetLayout const*() const;

        private:

            static_assert(std::is_pointer_v<VkDevice>);

            VkDevice _device;

            static_assert(std::is_pointer_v<VkDescriptorSetLayout>);

            VkDescriptorSetLayout _layout;
    };

}  // namespace lue::vulkan

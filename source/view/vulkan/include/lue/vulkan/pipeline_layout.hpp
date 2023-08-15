#pragma once
#include "lue/vulkan/structure.hpp"
#include <vulkan/vulkan.h>


namespace lue::vulkan {

    class PipelineLayout
    {
        public:

            using CreateInfo =
                Structure<VkPipelineLayoutCreateInfo, VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO>;


            PipelineLayout();

            PipelineLayout(VkDevice device, VkPipelineLayout pipeline_layout);

            PipelineLayout(PipelineLayout const&) = delete;

            PipelineLayout(PipelineLayout&& other);

            ~PipelineLayout();

            PipelineLayout& operator=(PipelineLayout const&) = delete;

            PipelineLayout& operator=(PipelineLayout&& other);

            bool is_valid() const;

            operator VkPipelineLayout();

        private:

            static_assert(std::is_pointer_v<VkDevice>);

            VkDevice _device;

            static_assert(std::is_pointer_v<VkPipelineLayout>);

            VkPipelineLayout _pipeline_layout;
    };

}  // namespace lue::vulkan

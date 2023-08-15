#pragma once
#include "lue/vulkan/structure.hpp"
#include <vulkan/vulkan.h>
#include <vector>


namespace lue::vulkan {

    class Pipeline
    {

        public:

            using ShaderStageCreateInfo = Structure<
                VkPipelineShaderStageCreateInfo,
                VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO>;

            using GraphicsPipelineCreateInfo =
                Structure<VkGraphicsPipelineCreateInfo, VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO>;

            using VertexInputStateCreateInfo = Structure<
                VkPipelineVertexInputStateCreateInfo,
                VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO>;

            using InputAssemblyStateCreateInfo = Structure<
                VkPipelineInputAssemblyStateCreateInfo,
                VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO>;


            Pipeline();

            Pipeline(VkDevice device, VkPipeline pipeline);

            Pipeline(Pipeline const&) = delete;

            Pipeline(Pipeline&& other);

            ~Pipeline();

            Pipeline& operator=(Pipeline const&) = delete;

            Pipeline& operator=(Pipeline&& other);

            bool is_valid() const;

            operator VkPipeline();

        private:

            static_assert(std::is_pointer_v<VkDevice>);

            VkDevice _device;

            static_assert(std::is_pointer_v<VkPipeline>);

            VkPipeline _pipeline;
    };

}  // namespace lue::vulkan

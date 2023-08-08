#pragma once
#include <vulkan/vulkan.h>
#include <vector>


namespace lue::vulkan {

    class Pipeline
    {

        public:

            class ShaderStageCreateInfo
            {

                public:

                    ShaderStageCreateInfo(VkShaderStageFlagBits const stage, VkShaderModule module);

                    ShaderStageCreateInfo(ShaderStageCreateInfo const&) = delete;

                    ShaderStageCreateInfo(ShaderStageCreateInfo&&) = default;

                    ~ShaderStageCreateInfo() = default;

                    ShaderStageCreateInfo& operator=(ShaderStageCreateInfo const&) = delete;

                    ShaderStageCreateInfo& operator=(ShaderStageCreateInfo&&) = default;

                    operator VkPipelineShaderStageCreateInfo const*() const;

                private:

                    VkPipelineShaderStageCreateInfo _create_info;
            };


            class GraphicsPipelineCreateInfo
            {

                public:

                    GraphicsPipelineCreateInfo();

                    GraphicsPipelineCreateInfo(GraphicsPipelineCreateInfo const&) = delete;

                    GraphicsPipelineCreateInfo(GraphicsPipelineCreateInfo&&) = default;

                    ~GraphicsPipelineCreateInfo() = default;

                    GraphicsPipelineCreateInfo& operator=(GraphicsPipelineCreateInfo const&) = delete;

                    GraphicsPipelineCreateInfo& operator=(GraphicsPipelineCreateInfo&&) = default;

                    operator VkGraphicsPipelineCreateInfo const*() const;

                    VkGraphicsPipelineCreateInfo& operator*();

                private:

                    VkGraphicsPipelineCreateInfo _create_info;
            };


            Pipeline();

            Pipeline(VkDevice device, VkPipeline pipeline);

            Pipeline(Pipeline const&) = delete;

            Pipeline(Pipeline&& other);

            ~Pipeline();

            Pipeline& operator=(Pipeline const&) = delete;

            Pipeline& operator=(Pipeline&& other);

            operator bool() const;

            operator VkPipeline() const;

        private:

            static_assert(std::is_pointer_v<VkDevice>);

            VkDevice _device;

            static_assert(std::is_pointer_v<VkPipeline>);

            VkPipeline _pipeline;
    };

}  // namespace lue::vulkan

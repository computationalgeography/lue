#include "lue/vulkan/pipeline.hpp"
#include <cassert>


namespace lue::vulkan {

    Pipeline::ShaderStageCreateInfo::ShaderStageCreateInfo(
        VkShaderStageFlagBits const stage, VkShaderModule module):

        _create_info{}

    {
        _create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        _create_info.stage = stage;
        _create_info.module = module;
        _create_info.pName = "main";
    }


    /*!
        @warning    Do not use the returned pointer after this instance has gone out of scope
    */
    Pipeline::ShaderStageCreateInfo::operator VkPipelineShaderStageCreateInfo const*() const
    {
        return &_create_info;
    }


    Pipeline::GraphicsPipelineCreateInfo::GraphicsPipelineCreateInfo():

        _create_info{}

    {
        _create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    }


    /*!
        @warning    Do not use the returned pointer after this instance has gone out of scope
    */
    Pipeline::GraphicsPipelineCreateInfo::operator VkGraphicsPipelineCreateInfo const*() const
    {
        return &_create_info;
    }


    VkGraphicsPipelineCreateInfo& Pipeline::GraphicsPipelineCreateInfo::operator*()
    {
        return _create_info;
    }


    Pipeline::Pipeline():

        _device{},
        _pipeline{}

    {
        assert(!*this);
    }


    Pipeline::Pipeline(VkDevice device, VkPipeline pipeline):

        _device{device},
        _pipeline{pipeline}

    {
        assert(*this);
    }


    Pipeline::Pipeline(Pipeline&& other):

        _device{std::move(other._device)},
        _pipeline{std::move(other._pipeline)}

    {
        other._device = VkDevice{};
        other._pipeline = VkPipeline{};

        assert(!other);
    }


    Pipeline::~Pipeline()
    {
        if (*this)
        {
            vkDestroyPipeline(_device, _pipeline, nullptr);
            _device = VkDevice{};
            _pipeline = VkPipeline{};
        }

        assert(!*this);
    }


    Pipeline& Pipeline::operator=(Pipeline&& other)
    {
        if (*this)
        {
            vkDestroyPipeline(_device, _pipeline, nullptr);
        }

        _device = std::move(other._device);
        _pipeline = std::move(other._pipeline);

        other._device = VkDevice{};
        other._pipeline = VkPipeline{};

        assert(!other);

        return *this;
    }


    Pipeline::operator bool() const
    {
        return _pipeline != VK_NULL_HANDLE;
    }


    Pipeline::operator VkPipeline() const
    {
        assert(*this);

        return _pipeline;
    }

}  // namespace lue::vulkan

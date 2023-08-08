#include "lue/vulkan/pipeline_layout.hpp"
#include <cassert>
#include <utility>


namespace lue::vulkan {

    PipelineLayout::CreateInfo::CreateInfo():

        _create_info{}

    {
        _create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        _create_info.setLayoutCount = 0;             // Optional
        _create_info.pSetLayouts = nullptr;          // Optional
        _create_info.pushConstantRangeCount = 0;     // Optional
        _create_info.pPushConstantRanges = nullptr;  // Optional
    }


    /*!
        @warning    Do not use the returned pointer after this instance has gone out of scope
    */
    PipelineLayout::CreateInfo::operator VkPipelineLayoutCreateInfo const*() const
    {
        return &_create_info;
    }


    PipelineLayout::PipelineLayout():

        _device{},
        _pipeline_layout{}

    {
        assert(!*this);
    }


    PipelineLayout::PipelineLayout(VkDevice device, VkPipelineLayout pipeline_layout):

        _device{device},
        _pipeline_layout{pipeline_layout}

    {
        assert(*this);
    }


    PipelineLayout::PipelineLayout(PipelineLayout&& other):

        _device{std::move(other._device)},
        _pipeline_layout{std::move(other._pipeline_layout)}

    {
        other._device = VkDevice{};
        other._pipeline_layout = VkPipelineLayout{};

        assert(!other);
    }


    PipelineLayout::~PipelineLayout()
    {
        if (*this)
        {
            vkDestroyPipelineLayout(_device, _pipeline_layout, nullptr);
            _device = VkDevice{};
            _pipeline_layout = VkPipelineLayout{};
        }

        assert(!*this);
    }


    PipelineLayout& PipelineLayout::operator=(PipelineLayout&& other)
    {
        if (*this)
        {
            vkDestroyPipelineLayout(_device, _pipeline_layout, nullptr);
        }

        _device = std::move(other._device);
        _pipeline_layout = std::move(other._pipeline_layout);

        other._device = VkDevice{};
        other._pipeline_layout = VkPipelineLayout{};

        assert(!other);

        return *this;
    }


    PipelineLayout::operator bool() const
    {
        return _pipeline_layout != VK_NULL_HANDLE;
    }


    PipelineLayout::operator VkPipelineLayout() const
    {
        assert(*this);

        return _pipeline_layout;
    }

}  // namespace lue::vulkan

#include "lue/vulkan/pipeline_layout.hpp"
#include <cassert>
#include <utility>


namespace lue::vulkan {

    PipelineLayout::PipelineLayout():

        _device{},
        _pipeline_layout{}

    {
        assert(!is_valid());
    }


    PipelineLayout::PipelineLayout(VkDevice device, VkPipelineLayout pipeline_layout):

        _device{device},
        _pipeline_layout{pipeline_layout}

    {
        assert(is_valid());
    }


    PipelineLayout::PipelineLayout(PipelineLayout&& other):

        _device{std::move(other._device)},
        _pipeline_layout{std::move(other._pipeline_layout)}

    {
        other._device = VkDevice{};
        other._pipeline_layout = VkPipelineLayout{};

        assert(!other.is_valid());
    }


    PipelineLayout::~PipelineLayout()
    {
        if (is_valid())
        {
            vkDestroyPipelineLayout(_device, _pipeline_layout, nullptr);

            _device = VkDevice{};
            _pipeline_layout = VkPipelineLayout{};
        }

        assert(!is_valid());
    }


    PipelineLayout& PipelineLayout::operator=(PipelineLayout&& other)
    {
        if (is_valid())
        {
            vkDestroyPipelineLayout(_device, _pipeline_layout, nullptr);
        }

        _device = std::move(other._device);
        _pipeline_layout = std::move(other._pipeline_layout);

        other._device = VkDevice{};
        other._pipeline_layout = VkPipelineLayout{};

        assert(!other.is_valid());

        return *this;
    }


    bool PipelineLayout::is_valid() const
    {
        return _pipeline_layout != VK_NULL_HANDLE;
    }


    PipelineLayout::operator VkPipelineLayout()
    {
        assert(is_valid());

        return _pipeline_layout;
    }

}  // namespace lue::vulkan

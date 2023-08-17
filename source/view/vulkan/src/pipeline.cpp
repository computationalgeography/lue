#include "lue/vulkan/pipeline.hpp"
#include <cassert>


namespace lue::vulkan {

    Pipeline::Pipeline():

        _device{},
        _pipeline{}

    {
        assert(!is_valid());
    }


    Pipeline::Pipeline(VkDevice device, VkPipeline pipeline):

        _device{device},
        _pipeline{pipeline}

    {
        assert(is_valid());
    }


    Pipeline::Pipeline(Pipeline&& other):

        _device{std::move(other._device)},
        _pipeline{std::move(other._pipeline)}

    {
        other._device = VkDevice{};
        other._pipeline = VkPipeline{};

        assert(!other.is_valid());
    }


    Pipeline::~Pipeline()
    {
        if (is_valid())
        {
            vkDestroyPipeline(_device, _pipeline, nullptr);

            _device = VkDevice{};
            _pipeline = VkPipeline{};
        }

        assert(!is_valid());
    }


    Pipeline& Pipeline::operator=(Pipeline&& other)
    {
        if (is_valid())
        {
            vkDestroyPipeline(_device, _pipeline, nullptr);
        }

        _device = std::move(other._device);
        _pipeline = std::move(other._pipeline);

        other._device = VkDevice{};
        other._pipeline = VkPipeline{};

        assert(!other.is_valid());

        return *this;
    }


    bool Pipeline::is_valid() const
    {
        return _pipeline != VK_NULL_HANDLE;
    }


    Pipeline::operator VkPipeline()
    {
        assert(is_valid());

        return _pipeline;
    }

}  // namespace lue::vulkan

#include "lue/vulkan/pipeline.hpp"
#include <cassert>


namespace lue::vulkan {

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

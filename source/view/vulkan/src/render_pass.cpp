#include "lue/vulkan/render_pass.hpp"
#include <cassert>
#include <utility>


namespace lue::vulkan {

    RenderPass::CreateInfo::CreateInfo():

        _create_info{}

    {
        _create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    }


    /*!
        @warning    Do not use the returned pointer after this instance has gone out of scope
    */
    RenderPass::CreateInfo::operator VkRenderPassCreateInfo const*() const
    {
        return &_create_info;
    }


    VkRenderPassCreateInfo& RenderPass::CreateInfo::operator*()
    {
        return _create_info;
    }


    RenderPass::RenderPass():

        _device{},
        _pipeline_layout{}

    {
        assert(!*this);
    }


    RenderPass::RenderPass(VkDevice device, VkRenderPass render_pass):

        _device{device},
        _pipeline_layout{render_pass}

    {
        assert(*this);
    }


    RenderPass::RenderPass(RenderPass&& other):

        _device{std::move(other._device)},
        _pipeline_layout{std::move(other._pipeline_layout)}

    {
        other._device = VkDevice{};
        other._pipeline_layout = VkRenderPass{};

        assert(!other);
    }


    RenderPass::~RenderPass()
    {
        if (*this)
        {
            vkDestroyRenderPass(_device, _pipeline_layout, nullptr);
            _device = VkDevice{};
            _pipeline_layout = VkRenderPass{};
        }

        assert(!*this);
    }


    RenderPass& RenderPass::operator=(RenderPass&& other)
    {
        if (*this)
        {
            vkDestroyRenderPass(_device, _pipeline_layout, nullptr);
        }

        _device = std::move(other._device);
        _pipeline_layout = std::move(other._pipeline_layout);

        other._device = VkDevice{};
        other._pipeline_layout = VkRenderPass{};

        assert(!other);

        return *this;
    }


    RenderPass::operator bool() const
    {
        return _pipeline_layout != VK_NULL_HANDLE;
    }


    RenderPass::operator VkRenderPass() const
    {
        assert(*this);

        return _pipeline_layout;
    }

}  // namespace lue::vulkan

#include "lue/vulkan/render_pass.hpp"
#include <cassert>
#include <utility>


namespace lue::vulkan {

    RenderPass::RenderPass():

        _device{},
        _render_pass{}

    {
        assert(!*this);
    }


    RenderPass::RenderPass(VkDevice device, VkRenderPass render_pass):

        _device{device},
        _render_pass{render_pass}

    {
        assert(*this);
    }


    RenderPass::RenderPass(RenderPass&& other):

        _device{std::move(other._device)},
        _render_pass{std::move(other._render_pass)}

    {
        other._device = VkDevice{};
        other._render_pass = VkRenderPass{};

        assert(!other);
    }


    RenderPass::~RenderPass()
    {
        if (*this)
        {
            vkDestroyRenderPass(_device, _render_pass, nullptr);
            _device = VkDevice{};
            _render_pass = VkRenderPass{};
        }

        assert(!*this);
    }


    RenderPass& RenderPass::operator=(RenderPass&& other)
    {
        if (*this)
        {
            vkDestroyRenderPass(_device, _render_pass, nullptr);
        }

        _device = std::move(other._device);
        _render_pass = std::move(other._render_pass);

        other._device = VkDevice{};
        other._render_pass = VkRenderPass{};

        assert(!other);

        return *this;
    }


    RenderPass::operator bool() const
    {
        return _render_pass != VK_NULL_HANDLE;
    }


    RenderPass::operator VkRenderPass() const
    {
        assert(*this);

        return _render_pass;
    }

}  // namespace lue::vulkan

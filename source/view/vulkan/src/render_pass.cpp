#include "lue/vulkan/render_pass.hpp"
#include <cassert>
#include <utility>


namespace lue::vulkan {

    RenderPass::RenderPass():

        _device{},
        _render_pass{}

    {
        assert(!is_valid());
    }


    RenderPass::RenderPass(VkDevice device, VkRenderPass render_pass):

        _device{device},
        _render_pass{render_pass}

    {
        assert(is_valid());
    }


    RenderPass::RenderPass(RenderPass&& other):

        _device{std::move(other._device)},
        _render_pass{std::move(other._render_pass)}

    {
        other._device = VkDevice{};
        other._render_pass = VkRenderPass{};

        assert(!other.is_valid());
    }


    RenderPass::~RenderPass()
    {
        if (is_valid())
        {
            vkDestroyRenderPass(_device, _render_pass, nullptr);

            _device = VkDevice{};
            _render_pass = VkRenderPass{};
        }

        assert(!is_valid());
    }


    RenderPass& RenderPass::operator=(RenderPass&& other)
    {
        if (is_valid())
        {
            vkDestroyRenderPass(_device, _render_pass, nullptr);
        }

        _device = std::move(other._device);
        _render_pass = std::move(other._render_pass);

        other._device = VkDevice{};
        other._render_pass = VkRenderPass{};

        assert(!other.is_valid());

        return *this;
    }


    bool RenderPass::is_valid() const
    {
        return _render_pass != VK_NULL_HANDLE;
    }


    RenderPass::operator VkRenderPass()
    {
        assert(is_valid());

        return _render_pass;
    }

}  // namespace lue::vulkan

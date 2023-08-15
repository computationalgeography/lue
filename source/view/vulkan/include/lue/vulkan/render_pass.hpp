#pragma once
#include "lue/vulkan/structure.hpp"
#include <vulkan/vulkan.h>


namespace lue::vulkan {

    class RenderPass
    {
        public:

            using CreateInfo = Structure<VkRenderPassCreateInfo, VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO>;

            using BeginInfo = Structure<VkRenderPassBeginInfo, VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO>;


            RenderPass();

            RenderPass(VkDevice device, VkRenderPass render_pass);

            RenderPass(RenderPass const&) = delete;

            RenderPass(RenderPass&& other);

            ~RenderPass();

            RenderPass& operator=(RenderPass const&) = delete;

            RenderPass& operator=(RenderPass&& other);

            bool is_valid() const;

            operator VkRenderPass();

        private:

            static_assert(std::is_pointer_v<VkDevice>);

            VkDevice _device;

            static_assert(std::is_pointer_v<VkRenderPass>);

            VkRenderPass _render_pass;
    };

}  // namespace lue::vulkan

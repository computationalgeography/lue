#pragma once
#include <vulkan/vulkan.h>
#include <type_traits>


namespace lue::vulkan {

    class RenderPass
    {
        public:

            class CreateInfo
            {

                public:

                    CreateInfo();

                    CreateInfo(CreateInfo const&) = delete;

                    CreateInfo(CreateInfo&&) = default;

                    ~CreateInfo() = default;

                    CreateInfo& operator=(CreateInfo const&) = delete;

                    CreateInfo& operator=(CreateInfo&&) = default;

                    operator VkRenderPassCreateInfo const*() const;

                    VkRenderPassCreateInfo& operator*();

                private:

                    VkRenderPassCreateInfo _create_info;
            };


            RenderPass();

            RenderPass(VkDevice device, VkRenderPass render_pass);

            RenderPass(RenderPass const&) = delete;

            RenderPass(RenderPass&& other);

            ~RenderPass();

            RenderPass& operator=(RenderPass const&) = delete;

            RenderPass& operator=(RenderPass&& other);

            operator bool() const;

            operator VkRenderPass() const;

        private:

            static_assert(std::is_pointer_v<VkDevice>);

            VkDevice _device;

            static_assert(std::is_pointer_v<VkRenderPass>);

            VkRenderPass _pipeline_layout;
    };

}  // namespace lue::vulkan

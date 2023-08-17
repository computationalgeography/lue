#pragma once
#include "lue/vulkan/buffer.hpp"
#include "lue/vulkan/pipeline.hpp"
#include "lue/vulkan/render_pass.hpp"
#include <vulkan/vulkan.h>
#include <cinttypes>


namespace lue::vulkan {

    class CommandBuffer
    {
        public:

            using AllocateInfo =
                Structure<VkCommandBufferAllocateInfo, VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO>;

            using BeginInfo =
                Structure<VkCommandBufferBeginInfo, VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO>;


            CommandBuffer();

            CommandBuffer(VkDevice device, VkCommandBuffer command_buffer);

            CommandBuffer(CommandBuffer const&) = delete;

            CommandBuffer(CommandBuffer&& other);

            ~CommandBuffer() = default;

            CommandBuffer& operator=(CommandBuffer const&) = delete;

            CommandBuffer& operator=(CommandBuffer&& other);

            bool is_valid() const;

            operator VkCommandBuffer();

            operator VkCommandBuffer const*() const;

            void begin(BeginInfo const& begin_info);

            void end();

            void begin_render_pass(RenderPass::BeginInfo const& begin_info, VkSubpassContents contents);

            void end_render_pass();

            void bind_pipeline(VkPipelineBindPoint bind_point, Pipeline& pipeline);

            void bind_vertex_buffer(Buffer& buffer);

            void set_viewport(VkViewport const* viewport);

            void set_viewport(
                std::uint32_t const first_view_port,
                std::uint32_t const nr_viewports,
                VkViewport const* viewports);

            void set_scissor(VkRect2D const* scissor);

            void set_scissor(
                std::uint32_t const first_scissor, std::uint32_t const nr_scissors, VkRect2D const* scissors);

            void draw(
                std::uint32_t const nr_vertices,
                std::uint32_t const nr_instances,
                std::uint32_t const first_vertex,
                std::uint32_t const first_instance);

        private:

            static_assert(std::is_pointer_v<VkDevice>);

            VkDevice _device;

            static_assert(std::is_pointer_v<VkCommandBuffer>);

            VkCommandBuffer _command_buffer;
    };

}  // namespace lue::vulkan

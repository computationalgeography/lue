#include "lue/vulkan/command_buffer.hpp"
#include "lue/vulkan/error.hpp"
#include <cassert>
#include <utility>


namespace lue::vulkan {

    CommandBuffer::CommandBuffer():

        _device{},
        _command_buffer{}

    {
        assert(!is_valid());
    }


    CommandBuffer::CommandBuffer(VkDevice device, VkCommandBuffer command_buffer):

        _device{device},
        _command_buffer{command_buffer}

    {
        assert(is_valid());
    }


    CommandBuffer::CommandBuffer(CommandBuffer&& other):

        _device{std::move(other._device)},
        _command_buffer{std::move(other._command_buffer)}

    {
        other._device = VkDevice{};
        other._command_buffer = VkCommandBuffer{};

        assert(!other.is_valid());
    }


    // CommandBuffer::~CommandBuffer()
    // {
    //     if (is_valid())
    //     {
    //         vkDestroyCommandBuffer(_device, _command_buffer, nullptr);
    //         _device = VkDevice{};
    //         _command_buffer = VkCommandBuffer{};
    //     }

    //     assert(!is_valid());
    // }


    CommandBuffer& CommandBuffer::operator=(CommandBuffer&& other)
    {
        // if (is_valid())
        // {
        //     vkDestroyCommandBuffer(_device, _command_buffer, nullptr);
        // }

        _device = std::move(other._device);
        _command_buffer = std::move(other._command_buffer);

        other._device = VkDevice{};
        other._command_buffer = VkCommandBuffer{};

        assert(!other.is_valid());

        return *this;
    }


    bool CommandBuffer::is_valid() const
    {
        return _command_buffer != VK_NULL_HANDLE;
    }


    CommandBuffer::operator VkCommandBuffer()
    {
        assert(is_valid());

        return _command_buffer;
    }


    CommandBuffer::operator VkCommandBuffer const*() const
    {
        assert(is_valid());

        return &_command_buffer;
    }


    void CommandBuffer::begin(BeginInfo const& begin_info)
    {
        VkResult result = vkBeginCommandBuffer(_command_buffer, begin_info);

        assert_result_is_ok(result);
    }


    void CommandBuffer::end()
    {
        VkResult result = vkEndCommandBuffer(_command_buffer);

        assert_result_is_ok(result);
    }


    void CommandBuffer::begin_render_pass(RenderPass::BeginInfo const& begin_info, VkSubpassContents contents)
    {
        vkCmdBeginRenderPass(_command_buffer, begin_info, contents);
    }


    void CommandBuffer::end_render_pass()
    {
        vkCmdEndRenderPass(_command_buffer);
    }


    void CommandBuffer::bind_pipeline(VkPipelineBindPoint bind_point, Pipeline& pipeline)
    {
        vkCmdBindPipeline(_command_buffer, bind_point, pipeline);
    }


    void CommandBuffer::set_viewport(VkViewport const* viewport)
    {
        set_viewport(0, 1, viewport);
    }


    void CommandBuffer::set_viewport(
        std::uint32_t const first_view_port, std::uint32_t const nr_viewports, VkViewport const* viewports)
    {
        vkCmdSetViewport(_command_buffer, first_view_port, nr_viewports, viewports);
    }


    void CommandBuffer::set_scissor(VkRect2D const* scissor)
    {
        set_scissor(0, 1, scissor);
    }


    void CommandBuffer::set_scissor(
        std::uint32_t const first_scissor, std::uint32_t const nr_scissors, VkRect2D const* scissors)
    {
        vkCmdSetScissor(_command_buffer, first_scissor, nr_scissors, scissors);
    }

    void CommandBuffer::draw(
        std::uint32_t const nr_vertices,
        std::uint32_t const nr_instances,
        std::uint32_t const first_vertex,
        std::uint32_t const first_instance)
    {
        vkCmdDraw(_command_buffer, nr_vertices, nr_instances, first_vertex, first_instance);
    }

}  // namespace lue::vulkan

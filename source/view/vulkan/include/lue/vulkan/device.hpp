#pragma once
#include "lue/vulkan/buffer.hpp"
#include "lue/vulkan/command_buffer.hpp"
#include "lue/vulkan/command_pool.hpp"
#include "lue/vulkan/descriptor_pool.hpp"
#include "lue/vulkan/descriptor_set_layout.hpp"
#include "lue/vulkan/fence.hpp"
#include "lue/vulkan/framebuffer.hpp"
#include "lue/vulkan/image_view.hpp"
#include "lue/vulkan/memory.hpp"
#include "lue/vulkan/physical_device.hpp"
#include "lue/vulkan/pipeline.hpp"
#include "lue/vulkan/pipeline_layout.hpp"
#include "lue/vulkan/queue.hpp"
#include "lue/vulkan/render_pass.hpp"
#include "lue/vulkan/semaphore.hpp"
#include "lue/vulkan/shader_module.hpp"
#include "lue/vulkan/swapchain.hpp"
#include <tuple>


namespace lue::vulkan {

    using CommandBuffers = std::vector<CommandBuffer>;


    class Device
    {

        public:

            using CreateInfo = Structure<VkDeviceCreateInfo, VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO>;

            using Fences = std::vector<Fence>;

            using Semaphores = std::vector<Semaphore>;


            Device();

            Device(Device const&) = delete;

            Device(Device&& other);

            Device(PhysicalDevice& physical_device, CreateInfo const& create_info);

            ~Device();

            Device& operator=(Device const&) = delete;

            Device& operator=(Device&& other);

            bool is_valid() const;

            operator VkDevice();

            void wait_idle() const;

            Queue queue(QueueFamily const& queue_family) const;

            Swapchain create_swapchain(Swapchain::CreateInfo const& create_info) const;

            ImageView create_image_view(ImageView::CreateInfo const& create_info) const;

            ShaderModule create_shader_module(ShaderModule::CreateInfo const& create_info) const;

            PipelineLayout create_pipeline_layout(PipelineLayout::CreateInfo const& create_info) const;

            RenderPass create_render_pass(RenderPass::CreateInfo const& create_info) const;

            Pipeline create_graphics_pipeline(Pipeline::GraphicsPipelineCreateInfo const& create_info) const;

            Framebuffer create_framebuffer(Framebuffer::CreateInfo const& create_info) const;

            CommandPool create_command_pool(CommandPool::CreateInfo const& create_info) const;

            Buffer create_buffer(Buffer::CreateInfo const& create_info) const;

            CommandBuffers allocate_command_buffers(CommandBuffer::AllocateInfo const& allocate_info) const;

            Semaphore create_semaphore(Semaphore::CreateInfo const& create_info) const;

            Semaphores create_semaphores(
                std::uint32_t const nr_semaphores, Semaphore::CreateInfo const& create_info) const;

            Fence create_fence(Fence::CreateInfo const& create_info) const;

            Fences create_fences(std::uint32_t const nr_fences, Fence::CreateInfo const& create_info) const;

            VkResult wait(Fence& fence);

            VkResult reset(Fence& fence);

            std::tuple<VkResult, std::uint32_t> acquire_next_image(
                Swapchain& swapchain, Semaphore& semaphore);

            Memory allocate_memory(Memory::AllocateInfo const& allocate_info);

            void* map_memory(
                Memory& memory,
                VkDeviceSize const offset,
                VkDeviceSize const size,
                VkMemoryMapFlags const flags);

            void unmap_memory(Memory& memory);

            DescriptorSetLayout create_descriptor_set_layout(
                DescriptorSetLayout::CreateInfo const& create_info);

            DescriptorPool create_descriptor_pool(DescriptorPool::CreateInfo const& create_info);


        private:

            static_assert(std::is_pointer_v<VkDevice>);

            VkDevice _device;
    };

}  // namespace lue::vulkan

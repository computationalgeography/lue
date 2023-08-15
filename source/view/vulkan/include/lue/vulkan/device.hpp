#pragma once
#include "lue/vulkan/command_buffer.hpp"
#include "lue/vulkan/command_pool.hpp"
#include "lue/vulkan/fence.hpp"
#include "lue/vulkan/framebuffer.hpp"
#include "lue/vulkan/image_view.hpp"
#include "lue/vulkan/physical_device.hpp"
#include "lue/vulkan/pipeline.hpp"
#include "lue/vulkan/pipeline_layout.hpp"
#include "lue/vulkan/render_pass.hpp"
#include "lue/vulkan/semaphore.hpp"
#include "lue/vulkan/shader_module.hpp"
#include "lue/vulkan/swapchain.hpp"


namespace lue::vulkan {

    class Device
    {

        public:

            using QueueCreateInfo =
                Structure<VkDeviceQueueCreateInfo, VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO>;

            using CreateInfo = Structure<VkDeviceCreateInfo, VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO>;

            using PresentInfo = Structure<VkPresentInfoKHR, VK_STRUCTURE_TYPE_PRESENT_INFO_KHR>;

            using CommandBuffers = std::vector<CommandBuffer>;

            using Fences = std::vector<Fence>;

            using Semaphores = std::vector<Semaphore>;


            class Queue
            {

                public:

                    Queue();

                    Queue(Queue const&) = delete;

                    Queue(Queue&& other);

                    Queue(VkQueue queue);

                    ~Queue() = default;

                    Queue& operator=(Queue const&) = delete;

                    Queue& operator=(Queue&& other);

                    bool is_valid() const;

                    operator VkQueue();

                    void present(PresentInfo const& present_info);

                private:

                    static_assert(std::is_pointer_v<VkQueue>);

                    VkQueue _queue;
            };


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

            Swapchain swapchain(Swapchain::CreateInfo const& create_info) const;

            ImageView image_view(ImageView::CreateInfo const& create_info) const;

            ShaderModule shader_module(ShaderModule::CreateInfo const& create_info) const;

            PipelineLayout pipeline_layout(PipelineLayout::CreateInfo const& create_info) const;

            RenderPass render_pass(RenderPass::CreateInfo const& create_info) const;

            Pipeline graphics_pipeline(Pipeline::GraphicsPipelineCreateInfo const& create_info) const;

            Framebuffer framebuffer(Framebuffer::CreateInfo const& create_info) const;

            CommandPool command_pool(CommandPool::CreateInfo const& create_info) const;

            CommandBuffers command_buffers(CommandBuffer::AllocateInfo const& allocate_info) const;

            Semaphore semaphore(Semaphore::CreateInfo const& create_info) const;

            Semaphores semaphores(
                std::uint32_t const nr_semaphores, Semaphore::CreateInfo const& create_info) const;

            Fence fence(Fence::CreateInfo const& create_info) const;

            Fences fences(std::uint32_t const nr_fences, Fence::CreateInfo const& create_info) const;


        private:

            static_assert(std::is_pointer_v<VkDevice>);

            VkDevice _device;
    };

}  // namespace lue::vulkan

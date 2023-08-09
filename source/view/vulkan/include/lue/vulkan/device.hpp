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

            class QueueCreateInfo
            {

                public:

                    QueueCreateInfo() = default;

                    QueueCreateInfo(QueueCreateInfo const&) = delete;

                    QueueCreateInfo(QueueCreateInfo&&) = default;

                    QueueCreateInfo(QueueFamily const& queue_family);

                    ~QueueCreateInfo() = default;

                    QueueCreateInfo& operator=(QueueCreateInfo const&) = delete;

                    QueueCreateInfo& operator=(QueueCreateInfo&&) = default;

                    operator VkDeviceQueueCreateInfo const*() const;

                private:

                    static_assert(!std::is_pointer_v<VkDeviceQueueCreateInfo>);

                    //! The number of queues we want for a single queue family
                    VkDeviceQueueCreateInfo _create_info;

                    std::vector<float> _queue_priorities;
            };


            class CreateInfo
            {

                public:

                    CreateInfo() = delete;

                    CreateInfo(CreateInfo const&) = delete;

                    CreateInfo(CreateInfo&&) = default;

                    CreateInfo(std::vector<QueueCreateInfo>&& queue_create_infos, Names&& extension_names);

                    CreateInfo(
                        std::vector<QueueCreateInfo>&& queue_create_infos,
                        PhysicalDevice::Features&& enabled_features,
                        Names&& extension_names);

                    ~CreateInfo() = default;

                    CreateInfo& operator=(CreateInfo const&) = delete;

                    CreateInfo& operator=(CreateInfo&&) = default;

                    operator VkDeviceCreateInfo const*() const;

                private:

                    std::vector<VkDeviceQueueCreateInfo> _queue_create_infos;

                    PhysicalDevice::Features _enabled_features;

                    Names _extension_names;

                    static_assert(!std::is_pointer_v<VkDeviceCreateInfo>);

                    VkDeviceCreateInfo _create_info;
            };


            class PresentInfo
            {

                public:

                    PresentInfo();

                    PresentInfo(PresentInfo const&) = delete;

                    PresentInfo(PresentInfo&&) = default;

                    ~PresentInfo() = default;

                    PresentInfo& operator=(PresentInfo const&) = delete;

                    PresentInfo& operator=(PresentInfo&&) = default;

                    operator VkPresentInfoKHR const*() const;

                    VkPresentInfoKHR& operator*();

                private:

                    static_assert(!std::is_pointer_v<VkPresentInfoKHR>);

                    VkPresentInfoKHR _present_info;
            };


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

                    operator bool() const;

                    operator VkQueue() const;

                    void present(PresentInfo const& present_info);

                private:

                    static_assert(std::is_pointer_v<VkQueue>);

                    VkQueue _queue;
            };


            Device();

            Device(Device const&) = delete;

            Device(Device&& other);

            Device(PhysicalDevice const& physical_device, CreateInfo const& create_info);

            ~Device();

            Device& operator=(Device const&) = delete;

            Device& operator=(Device&& other);

            operator bool() const;

            operator VkDevice() const;

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

            CommandBuffer command_buffer(CommandBuffer::AllocateInfo const& allocate_info) const;

            Semaphore semaphore(Semaphore::CreateInfo const& create_info) const;

            Fence fence(Fence::CreateInfo const& create_info) const;

        private:

            static_assert(std::is_pointer_v<VkDevice>);

            VkDevice _device;
    };

}  // namespace lue::vulkan

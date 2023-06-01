#pragma once
#include "lue/vulkan/physical_device.hpp"


namespace lue::vulkan {

    class Device
    {

        public:

            class QueueCreateInfo
            {

                public:

                    QueueCreateInfo() = delete;

                    QueueCreateInfo(QueueCreateInfo const&) = delete;

                    QueueCreateInfo(QueueCreateInfo&&) = default;

                    QueueCreateInfo(PhysicalDevice::QueueFamily const& queue_family);

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

                    CreateInfo(QueueCreateInfo&& queue_create_info);

                    CreateInfo(
                        QueueCreateInfo&& queue_create_info, PhysicalDevice::Features&& enabled_features);

                    ~CreateInfo() = default;

                    CreateInfo& operator=(CreateInfo const&) = delete;

                    CreateInfo& operator=(CreateInfo&&) = default;

                    operator VkDeviceCreateInfo const*() const;

                private:

                    PhysicalDevice::Features _enabled_features;

                    static_assert(!std::is_pointer_v<VkDeviceCreateInfo>);

                    VkDeviceCreateInfo _create_info;
            };


            class Queue
            {

                public:

                    Queue() = delete;

                    Queue(Queue const&) = delete;

                    Queue(Queue&&) = default;

                    Queue(VkQueue queue);

                    ~Queue() = default;

                    Queue& operator=(Queue const&) = delete;

                    Queue& operator=(Queue&&) = default;

                    operator VkQueue() const;

                private:

                    static_assert(std::is_pointer_v<VkQueue>);

                    VkQueue _queue;
            };


            Device() = delete;

            Device(Device const&) = delete;

            Device(Device&&) = default;

            Device(PhysicalDevice const& physical_device, CreateInfo const& create_info);

            ~Device();

            Device& operator=(Device const&) = delete;

            Device& operator=(Device&&) = default;

            Queue queue(PhysicalDevice::QueueFamily const& queue_family);

        private:

            static_assert(std::is_pointer_v<VkDevice>);

            VkDevice _device;
    };

}  // namespace lue::vulkan

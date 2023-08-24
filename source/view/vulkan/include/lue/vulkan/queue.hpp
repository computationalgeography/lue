#pragma once
#include "lue/vulkan/structure.hpp"
#include <vulkan/vulkan.h>


namespace lue::vulkan {

    class Queue
    {

        public:

            using CreateInfo = Structure<VkDeviceQueueCreateInfo, VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO>;

            using PresentInfo = Structure<VkPresentInfoKHR, VK_STRUCTURE_TYPE_PRESENT_INFO_KHR>;


            Queue();

            Queue(Queue const&) = delete;

            Queue(Queue&& other);

            Queue(VkQueue queue);

            // No need to destroy the layered queue
            ~Queue() = default;

            Queue& operator=(Queue const&) = delete;

            Queue& operator=(Queue&& other);

            bool is_valid() const;

            operator VkQueue();

            VkResult present(PresentInfo const& present_info);

        private:

            static_assert(std::is_pointer_v<VkQueue>);

            VkQueue _queue;
    };

}  // namespace lue::vulkan

#pragma once
#include "lue/vulkan/structure.hpp"
#include <vulkan/vulkan.h>


namespace lue::vulkan {

    class Semaphore
    {
        public:

            using CreateInfo = Structure<VkSemaphoreCreateInfo, VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO>;


            Semaphore();

            Semaphore(VkDevice device, VkSemaphore semaphore);

            Semaphore(Semaphore const&) = delete;

            Semaphore(Semaphore&& other);

            ~Semaphore();

            Semaphore& operator=(Semaphore const&) = delete;

            Semaphore& operator=(Semaphore&& other);

            bool is_valid() const;

            operator VkSemaphore();

        private:

            static_assert(std::is_pointer_v<VkDevice>);

            VkDevice _device;

            static_assert(std::is_pointer_v<VkSemaphore>);

            VkSemaphore _semaphore;
    };

}  // namespace lue::vulkan

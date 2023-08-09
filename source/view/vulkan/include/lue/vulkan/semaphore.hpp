#pragma once
#include <vulkan/vulkan.h>
#include <type_traits>


namespace lue::vulkan {

    class Semaphore
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

                    operator VkSemaphoreCreateInfo const*() const;

                    VkSemaphoreCreateInfo& operator*();

                private:

                    VkSemaphoreCreateInfo _create_info;
            };


            Semaphore();

            Semaphore(VkDevice device, VkSemaphore render_pass);

            Semaphore(Semaphore const&) = delete;

            Semaphore(Semaphore&& other);

            ~Semaphore();

            Semaphore& operator=(Semaphore const&) = delete;

            Semaphore& operator=(Semaphore&& other);

            operator bool() const;

            operator VkSemaphore() const;

        private:

            static_assert(std::is_pointer_v<VkDevice>);

            VkDevice _device;

            static_assert(std::is_pointer_v<VkSemaphore>);

            VkSemaphore _semaphore;
    };

}  // namespace lue::vulkan

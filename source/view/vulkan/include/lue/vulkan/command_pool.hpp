#pragma once
#include <vulkan/vulkan.h>
#include <type_traits>


namespace lue::vulkan {

    class CommandPool
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

                    operator VkCommandPoolCreateInfo const*() const;

                    VkCommandPoolCreateInfo& operator*();

                private:

                    VkCommandPoolCreateInfo _create_info;
            };


            CommandPool();

            CommandPool(VkDevice device, VkCommandPool command_pool);

            CommandPool(CommandPool const&) = delete;

            CommandPool(CommandPool&& other);

            ~CommandPool();

            CommandPool& operator=(CommandPool const&) = delete;

            CommandPool& operator=(CommandPool&& other);

            operator bool() const;

            operator VkCommandPool() const;

        private:

            static_assert(std::is_pointer_v<VkDevice>);

            VkDevice _device;

            static_assert(std::is_pointer_v<VkCommandPool>);

            VkCommandPool _command_pool;
    };

}  // namespace lue::vulkan

#pragma once
#include "lue/vulkan/structure.hpp"
#include <vulkan/vulkan.h>


namespace lue::vulkan {

    class CommandPool
    {
        public:

            using CreateInfo = Structure<VkCommandPoolCreateInfo, VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO>;


            CommandPool();

            CommandPool(VkDevice device, VkCommandPool command_pool);

            CommandPool(CommandPool const&) = delete;

            CommandPool(CommandPool&& other);

            ~CommandPool();

            CommandPool& operator=(CommandPool const&) = delete;

            CommandPool& operator=(CommandPool&& other);

            bool is_valid() const;

            operator VkCommandPool();

        private:

            static_assert(std::is_pointer_v<VkDevice>);

            VkDevice _device;

            static_assert(std::is_pointer_v<VkCommandPool>);

            VkCommandPool _command_pool;
    };

}  // namespace lue::vulkan

#pragma once
#include "lue/vulkan/structure.hpp"
#include <vulkan/vulkan.h>


namespace lue::vulkan {

    class Fence
    {
        public:

            using CreateInfo = Structure<VkFenceCreateInfo, VK_STRUCTURE_TYPE_FENCE_CREATE_INFO>;

            Fence();

            Fence(VkDevice device, VkFence render_pass);

            Fence(Fence const&) = delete;

            Fence(Fence&& other);

            ~Fence();

            Fence& operator=(Fence const&) = delete;

            Fence& operator=(Fence&& other);

            bool is_valid() const;

            operator VkFence();

            operator VkFence const*() const;

        private:

            static_assert(std::is_pointer_v<VkDevice>);

            VkDevice _device;

            static_assert(std::is_pointer_v<VkFence>);

            VkFence _fence;
    };

}  // namespace lue::vulkan

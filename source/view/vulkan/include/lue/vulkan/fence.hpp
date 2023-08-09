#pragma once
#include <vulkan/vulkan.h>
#include <type_traits>


namespace lue::vulkan {

    class Fence
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

                    operator VkFenceCreateInfo const*() const;

                    VkFenceCreateInfo& operator*();

                private:

                    VkFenceCreateInfo _create_info;
            };


            Fence();

            Fence(VkDevice device, VkFence render_pass);

            Fence(Fence const&) = delete;

            Fence(Fence&& other);

            ~Fence();

            Fence& operator=(Fence const&) = delete;

            Fence& operator=(Fence&& other);

            operator bool() const;

            operator VkFence() const;

            operator VkFence const*() const;

        private:

            static_assert(std::is_pointer_v<VkDevice>);

            VkDevice _device;

            static_assert(std::is_pointer_v<VkFence>);

            VkFence _fence;
    };

}  // namespace lue::vulkan

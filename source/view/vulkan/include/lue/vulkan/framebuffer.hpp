#pragma once
#include <vulkan/vulkan.h>
#include <type_traits>
#include <vector>


namespace lue::vulkan {

    class Framebuffer
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

                    operator VkFramebufferCreateInfo const*() const;

                    VkFramebufferCreateInfo& operator*();

                private:

                    VkFramebufferCreateInfo _create_info;
            };


            Framebuffer();

            Framebuffer(VkDevice device, VkFramebuffer framebuffer);

            Framebuffer(Framebuffer const&) = delete;

            Framebuffer(Framebuffer&& other);

            ~Framebuffer();

            Framebuffer& operator=(Framebuffer const&) = delete;

            Framebuffer& operator=(Framebuffer&& other);

            operator bool() const;

            operator VkFramebuffer() const;

        private:

            static_assert(std::is_pointer_v<VkDevice>);

            VkDevice _device;

            static_assert(std::is_pointer_v<VkFramebuffer>);

            VkFramebuffer _framebuffer;
    };


    using Framebuffers = std::vector<Framebuffer>;

}  // namespace lue::vulkan

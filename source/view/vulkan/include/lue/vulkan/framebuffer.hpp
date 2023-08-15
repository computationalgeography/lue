#pragma once
#include "lue/vulkan/structure.hpp"
#include <vulkan/vulkan.h>
#include <vector>


namespace lue::vulkan {

    class Framebuffer
    {
        public:

            using CreateInfo = Structure<VkFramebufferCreateInfo, VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO>;


            Framebuffer();

            Framebuffer(VkDevice device, VkFramebuffer framebuffer);

            Framebuffer(Framebuffer const&) = delete;

            Framebuffer(Framebuffer&& other);

            ~Framebuffer();

            Framebuffer& operator=(Framebuffer const&) = delete;

            Framebuffer& operator=(Framebuffer&& other);

            bool is_valid() const;

            operator VkFramebuffer();

        private:

            static_assert(std::is_pointer_v<VkDevice>);

            VkDevice _device;

            static_assert(std::is_pointer_v<VkFramebuffer>);

            VkFramebuffer _framebuffer;
    };


    using Framebuffers = std::vector<Framebuffer>;

}  // namespace lue::vulkan

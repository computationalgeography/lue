#pragma once
#include "lue/vulkan/structure.hpp"
#include <vulkan/vulkan.h>
#include <string>
#include <vector>


namespace lue::vulkan {

    class ShaderModule
    {

        public:

            using Bytes = std::vector<char>;

            using CreateInfo =
                Structure<VkShaderModuleCreateInfo, VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO>;


            static Bytes read_file(std::string const& pathname);


            ShaderModule();

            ShaderModule(VkDevice device, VkShaderModule shader_module);

            ShaderModule(ShaderModule const&) = delete;

            ShaderModule(ShaderModule&& other);

            ~ShaderModule();

            ShaderModule& operator=(ShaderModule const&) = delete;

            ShaderModule& operator=(ShaderModule&& other);

            bool is_valid() const;

            operator VkShaderModule();

        private:

            static_assert(std::is_pointer_v<VkDevice>);

            VkDevice _device;

            static_assert(std::is_pointer_v<VkShaderModule>);

            VkShaderModule _shader_module;
    };

}  // namespace lue::vulkan

#pragma once
#include <vulkan/vulkan.h>
#include <string>
#include <vector>


namespace lue::vulkan {

    class ShaderModule
    {

        public:

            using Bytes = std::vector<char>;


            class CreateInfo
            {

                public:

                    CreateInfo(Bytes const& bytes);

                    CreateInfo(CreateInfo const&) = delete;

                    CreateInfo(CreateInfo&&) = default;

                    ~CreateInfo() = default;

                    CreateInfo& operator=(CreateInfo const&) = delete;

                    CreateInfo& operator=(CreateInfo&&) = default;

                    operator VkShaderModuleCreateInfo const*() const;

                private:

                    VkShaderModuleCreateInfo _create_info;
            };


            static Bytes read_file(std::string const& pathname);


            ShaderModule();

            ShaderModule(VkDevice device, VkShaderModule shader_module);

            ShaderModule(ShaderModule const&) = delete;

            ShaderModule(ShaderModule&& other);

            ~ShaderModule();

            ShaderModule& operator=(ShaderModule const&) = delete;

            ShaderModule& operator=(ShaderModule&& other);

            operator bool() const;

            operator VkShaderModule() const;

        private:

            static_assert(std::is_pointer_v<VkDevice>);

            VkDevice _device;

            static_assert(std::is_pointer_v<VkShaderModule>);

            VkShaderModule _shader_module;
    };

}  // namespace lue::vulkan

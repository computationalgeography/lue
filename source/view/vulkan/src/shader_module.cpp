#include "lue/vulkan/shader_module.hpp"
#include <fmt/format.h>
#include <cassert>
#include <fstream>


namespace lue::vulkan {

    ShaderModule::Bytes ShaderModule::read_file(std::string const& pathname)
    {
        std::ifstream file(pathname, std::ios::ate | std::ios::binary);

        if (!file.is_open())
        {
            throw std::runtime_error(fmt::format("Failed to open file: {}", pathname));
        }

        std::size_t const nr_bytes = file.tellg();

        Bytes buffer(nr_bytes);
        file.seekg(0);
        file.read(buffer.data(), nr_bytes);
        file.close();

        return buffer;
    }


    ShaderModule::ShaderModule():

        _device{},
        _shader_module{}

    {
        assert(!*this);
    }


    ShaderModule::ShaderModule(VkDevice device, VkShaderModule shader_module):

        _device{device},
        _shader_module{shader_module}

    {
        assert(*this);
    }


    ShaderModule::ShaderModule(ShaderModule&& other):

        _device{std::move(other._device)},
        _shader_module{std::move(other._shader_module)}

    {
        other._device = VkDevice{};
        other._shader_module = VkShaderModule{};

        assert(!other);
    }


    ShaderModule::~ShaderModule()
    {
        if (*this)
        {
            vkDestroyShaderModule(_device, _shader_module, nullptr);
            _device = VkDevice{};
            _shader_module = VkShaderModule{};
        }

        assert(!*this);
    }


    ShaderModule& ShaderModule::operator=(ShaderModule&& other)
    {
        if (*this)
        {
            vkDestroyShaderModule(_device, _shader_module, nullptr);
        }

        _device = std::move(other._device);
        _shader_module = std::move(other._shader_module);

        other._device = VkDevice{};
        other._shader_module = VkShaderModule{};

        assert(!other);

        return *this;
    }


    ShaderModule::operator bool() const
    {
        return _shader_module != VK_NULL_HANDLE;
    }


    ShaderModule::operator VkShaderModule() const
    {
        assert(*this);

        return _shader_module;
    }

}  // namespace lue::vulkan

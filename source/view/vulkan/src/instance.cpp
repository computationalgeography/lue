#include "lue/vulkan/instance.hpp"
#include <stdexcept>


namespace lue::vulkan {

    Instance::Instance(InstanceCreateInfo&& instance_create_info):

        _instance_create_info{std::move(instance_create_info)},
        _instance{}

    {
        VkResult result = ::vkCreateInstance(_instance_create_info, nullptr, &_instance);

        if (result != VK_SUCCESS)
        {
            throw std::runtime_error{"Failed to create Vulkan instance"};
        }
    }


    Instance::~Instance()
    {
        ::vkDestroyInstance(_instance, nullptr);
    }

}  // namespace lue::vulkan

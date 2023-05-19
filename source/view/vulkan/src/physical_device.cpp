#include "lue/vulkan/physical_device.hpp"
#include <utility>


namespace lue::vulkan {

    PhysicalDevice::Properties::Properties(VkPhysicalDeviceProperties&& properties):

        _properties{std::move(properties)}

    {
    }


    PhysicalDevice::PhysicalDevice(VkPhysicalDevice&& physical_device):

        _physical_device{std::move(physical_device)}

    {
    }


    PhysicalDevice::Properties PhysicalDevice::properties() const
    {
        VkPhysicalDeviceProperties properties;

        ::vkGetPhysicalDeviceProperties(_physical_device, &properties);

        return PhysicalDevice::Properties{std::move(properties)};
    }

}  // namespace lue::vulkan

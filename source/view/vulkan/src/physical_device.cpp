#include "lue/vulkan/physical_device.hpp"
#include <utility>


namespace lue::vulkan {

    PhysicalDevice::Properties::Properties(VkPhysicalDeviceProperties&& properties):

        _properties{std::move(properties)}

    {
    }


    std::string PhysicalDevice::Properties::device_name() const
    {
        return _properties.deviceName;
    }


    VkPhysicalDeviceType PhysicalDevice::Properties::device_type() const
    {
        return _properties.deviceType;
    }


    PhysicalDevice::Features::Features(VkPhysicalDeviceFeatures&& features):

        _features{std::move(features)}

    {
    }


    bool PhysicalDevice::Features::has_geometry_shader() const
    {
        return _features.geometryShader;
    }


    PhysicalDevice::QueueFamily::Properties::Properties(VkQueueFamilyProperties&& properties):

        _properties{std::move(properties)}

    {
    }


    bool PhysicalDevice::QueueFamily::Properties::graphics() const
    {
        return _properties.queueFlags & VK_QUEUE_GRAPHICS_BIT;
    }


    PhysicalDevice::PhysicalDevice(VkPhysicalDevice physical_device):

        _physical_device{physical_device}

    {
    }


    PhysicalDevice::Properties PhysicalDevice::properties() const
    {
        VkPhysicalDeviceProperties properties;

        ::vkGetPhysicalDeviceProperties(_physical_device, &properties);

        return PhysicalDevice::Properties{std::move(properties)};
    }


    PhysicalDevice::Features PhysicalDevice::features() const
    {
        VkPhysicalDeviceFeatures features;

        ::vkGetPhysicalDeviceFeatures(_physical_device, &features);

        return features;
    }

}  // namespace lue::vulkan

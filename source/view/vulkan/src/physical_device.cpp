#include "lue/vulkan/physical_device.hpp"
#include <algorithm>
#include <cassert>
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


    PhysicalDevice::Features::operator VkPhysicalDeviceFeatures const*() const
    {
        return &_features;
    }


    bool PhysicalDevice::Features::has_geometry_shader() const
    {
        return _features.geometryShader;
    }


    PhysicalDevice::QueueFamily::Properties::Properties():

        _properties{}

    {
    }


    PhysicalDevice::QueueFamily::Properties::Properties(VkQueueFamilyProperties&& properties):

        _properties{std::move(properties)}

    {
    }


    bool PhysicalDevice::QueueFamily::Properties::graphics() const
    {
        return _properties.queueFlags & VK_QUEUE_GRAPHICS_BIT;
    }


    PhysicalDevice::QueueFamily::QueueFamily():

        _idx{(std::uint32_t)-1}

    {
    }


    PhysicalDevice::QueueFamily::QueueFamily(std::uint32_t const idx):

        _idx{idx}

    {
    }


    std::uint32_t PhysicalDevice::QueueFamily::idx() const
    {
        return _idx;
    }


    PhysicalDevice::PhysicalDevice(VkPhysicalDevice physical_device):

        _physical_device{physical_device}

    {
    }


    PhysicalDevice::operator VkPhysicalDevice() const
    {
        return _physical_device;
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


    PhysicalDevice::QueueFamilyProperties PhysicalDevice::queue_family_properties() const
    {
        std::uint32_t nr_queue_families;

        ::vkGetPhysicalDeviceQueueFamilyProperties(_physical_device, &nr_queue_families, nullptr);

        std::vector<VkQueueFamilyProperties> properties1(nr_queue_families);

        ::vkGetPhysicalDeviceQueueFamilyProperties(_physical_device, &nr_queue_families, properties1.data());

        PhysicalDevice::QueueFamilyProperties properties2(nr_queue_families);

        std::transform(
            properties1.begin(),
            properties1.end(),
            properties2.begin(),
            [](auto const& properties) { return properties; });

        return properties2;
    }

}  // namespace lue::vulkan

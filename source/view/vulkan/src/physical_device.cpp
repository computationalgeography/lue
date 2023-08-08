#include "lue/vulkan/physical_device.hpp"
#include "lue/vulkan/error.hpp"
#include <algorithm>
#include <cassert>
#include <cstring>
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


    /*!
        @warning    Do not use the returned pointer after this instance has gone out of scope
    */
    PhysicalDevice::Features::operator VkPhysicalDeviceFeatures const*() const
    {
        return &_features;
    }


    bool PhysicalDevice::Features::has_geometry_shader() const
    {
        return _features.geometryShader;
    }


    PhysicalDevice::SurfaceProperties::SurfaceProperties(
        VkSurfaceCapabilitiesKHR&& capabilities, Formats&& formats, PresentModes&& present_modes):

        _capabilities{std::move(capabilities)},
        _formats{std::move(formats)},
        _present_modes{std::move(present_modes)}

    {
    }


    VkSurfaceCapabilitiesKHR const& PhysicalDevice::SurfaceProperties::capabilities() const
    {
        return _capabilities;
    }


    PhysicalDevice::SurfaceProperties::Formats const& PhysicalDevice::SurfaceProperties::formats() const
    {
        return _formats;
    }


    PhysicalDevice::SurfaceProperties::PresentModes const& PhysicalDevice::SurfaceProperties::present_modes()
        const
    {
        return _present_modes;
    }


    PhysicalDevice::PhysicalDevice():

        _physical_device{}

    {
        assert(!*this);
    }


    PhysicalDevice::PhysicalDevice(VkPhysicalDevice physical_device):

        _physical_device{physical_device}

    {
    }


    PhysicalDevice::PhysicalDevice(PhysicalDevice&& other):

        _physical_device{std::move(other._physical_device)}

    {
        other._physical_device = VkPhysicalDevice{};

        assert(!other);
    }


    PhysicalDevice& PhysicalDevice::operator=(PhysicalDevice&& other)
    {
        _physical_device = std::move(other._physical_device);

        other._physical_device = VkPhysicalDevice{};

        assert(!other);

        return *this;
    }


    PhysicalDevice::operator bool() const
    {
        return _physical_device != VK_NULL_HANDLE;
    }


    /*!
        @warning    Do not use the returned pointer after this instance has gone out of scope
    */
    PhysicalDevice::operator VkPhysicalDevice() const
    {
        assert(*this);

        return _physical_device;
    }


    PhysicalDevice::Properties PhysicalDevice::properties() const
    {
        assert(*this);

        VkPhysicalDeviceProperties properties;

        ::vkGetPhysicalDeviceProperties(_physical_device, &properties);

        return PhysicalDevice::Properties{std::move(properties)};
    }


    PhysicalDevice::Features PhysicalDevice::features() const
    {
        assert(*this);

        VkPhysicalDeviceFeatures features;

        ::vkGetPhysicalDeviceFeatures(_physical_device, &features);

        return features;
    }


    QueueFamilyProperties PhysicalDevice::queue_family_properties() const
    {
        assert(*this);

        std::uint32_t nr_queue_families;

        ::vkGetPhysicalDeviceQueueFamilyProperties(_physical_device, &nr_queue_families, nullptr);

        std::vector<VkQueueFamilyProperties> properties1(nr_queue_families);

        ::vkGetPhysicalDeviceQueueFamilyProperties(_physical_device, &nr_queue_families, properties1.data());

        QueueFamilyProperties properties2(nr_queue_families);

        std::transform(
            properties1.begin(),
            properties1.end(),
            properties2.begin(),
            [](auto const& properties) { return properties; });

        return properties2;
    }


    ExtensionProperties PhysicalDevice::extension_properties() const
    {
        assert(*this);

        std::uint32_t nr_properties{0};

        ::vkEnumerateDeviceExtensionProperties(_physical_device, nullptr, &nr_properties, nullptr);

        ExtensionProperties properties(nr_properties);

        VkResult result{::vkEnumerateDeviceExtensionProperties(
            _physical_device, nullptr, &nr_properties, properties.data())};
        assert_result_is_ok(result);

        return properties;
    }


    bool PhysicalDevice::extension_available(ExtensionProperties const& properties, std::string const& name)
    {
        bool is_available{false};

        for (auto const& p : properties)
        {
            if (std::strcmp(p.extensionName, name.c_str()) == 0)
            {
                is_available = true;
                break;
            }
        }

        return is_available;
    }


    bool PhysicalDevice::extensions_available(Names const& names) const
    {
        ExtensionProperties const properties{extension_properties()};

        bool are_available{true};

        for (auto const& name : names)
        {
            if (!extension_available(properties, name))
            {
                are_available = false;
                break;
            }
        }

        return are_available;
    }


    bool PhysicalDevice::has_surface_support(QueueFamily const& queue_family, Surface const& surface) const
    {
        assert(*this);

        VkBool32 support = false;

        ::vkGetPhysicalDeviceSurfaceSupportKHR(_physical_device, queue_family, surface, &support);

        return support;
    }


    PhysicalDevice::SurfaceProperties PhysicalDevice::surface_properties(Surface const& surface) const
    {
        assert(*this);

        VkSurfaceCapabilitiesKHR capabilities;

        ::vkGetPhysicalDeviceSurfaceCapabilitiesKHR(_physical_device, surface, &capabilities);

        std::uint32_t nr_formats{};
        SurfaceProperties::Formats formats;

        ::vkGetPhysicalDeviceSurfaceFormatsKHR(_physical_device, surface, &nr_formats, nullptr);

        if (nr_formats > 0)
        {
            formats.resize(nr_formats);
            ::vkGetPhysicalDeviceSurfaceFormatsKHR(_physical_device, surface, &nr_formats, formats.data());
        }

        std::uint32_t nr_present_modes{};
        SurfaceProperties::PresentModes present_modes;

        ::vkGetPhysicalDeviceSurfacePresentModesKHR(_physical_device, surface, &nr_present_modes, nullptr);

        if (nr_present_modes > 0)
        {
            present_modes.resize(nr_present_modes);
            ::vkGetPhysicalDeviceSurfacePresentModesKHR(
                _physical_device, surface, &nr_present_modes, present_modes.data());
        }

        return {std::move(capabilities), std::move(formats), std::move(present_modes)};
    }

}  // namespace lue::vulkan

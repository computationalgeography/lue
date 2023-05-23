#include "lue/vulkan/instance.hpp"
#include "lue/vulkan/error.hpp"
#include <algorithm>
#include <cstring>


namespace lue::vulkan {

    Instance::CreateInfo::CreateInfo(
        VkInstanceCreateFlags const flags,
        ApplicationInfo&& application_info,
        Names const& enabled_layer_names,
        Names const& enabled_extension_names):

        _application_info{std::move(application_info)},
        _create_info{}

    {
        _create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        _create_info.flags = flags;
        _create_info.pApplicationInfo = _application_info;

        // It is assumed that the character buffers themselves (not the vector) stay alive
        _create_info.enabledLayerCount = enabled_layer_names.size();
        _create_info.ppEnabledLayerNames = enabled_layer_names.data();

        _create_info.enabledExtensionCount = enabled_extension_names.size();
        _create_info.ppEnabledExtensionNames = enabled_extension_names.data();
    }


    Instance::CreateInfo::CreateInfo(
        ApplicationInfo&& application_info,
        Names const& enabled_layer_names,
        Names const& enabled_extension_names):

        Instance::CreateInfo{
            VkInstanceCreateFlags{},
            std::move(application_info),
            enabled_layer_names,
            enabled_extension_names}

    {
    }


    Instance::CreateInfo::CreateInfo(ApplicationInfo&& application_info):

        Instance::CreateInfo{VkInstanceCreateFlags{}, std::move(application_info), Names{}, Names{}}

    {
    }


    // Instance::ExtensionProperties::ExtensionProperties():

    //     _properties{}

    // {
    //     std::uint32_t nr_extensions{0};

    //     ::vkEnumerateInstanceExtensionProperties(nullptr, &nr_extensions, nullptr);

    //     _properties.resize(nr_extensions);

    //     VkResult result{
    //         ::vkEnumerateInstanceExtensionProperties(nullptr, &nr_extensions, _properties.data())};

    //     // VK_ERROR_OUT_OF_HOST_MEMORY
    //     // VK_ERROR_OUT_OF_DEVICE_MEMORY
    //     // VK_ERROR_LAYER_NOT_PRESENT
    //     assert_result_is_ok(result);
    // }


    // Names Instance::ExtensionProperties::extension_names() const
    // {
    //     Names names(_properties.size());

    //     std::transform(_properties.begin(), _properties.end(), names.begin(),
    //             [](auto const& properties)
    //             {
    //                 return properties.extensionName;
    //             }
    //         );

    //     return names;
    // }


    // bool Instance::ExtensionProperties::extension_available(std::string const& name) const
    // {
    //     for (auto const& properties: _properties)
    //     {
    //         if (std::strcmp(properties.extensionName, name.c_str()) == 0)
    //         {
    //             return true;
    //         }
    //     }

    //     return false;
    // }


    namespace {

        ExtensionProperties extension_properties(char const* layer_name)
        {
            std::uint32_t nr_properties{0};

            ::vkEnumerateInstanceExtensionProperties(layer_name, &nr_properties, nullptr);

            ExtensionProperties properties(nr_properties);

            VkResult result{
                ::vkEnumerateInstanceExtensionProperties(nullptr, &nr_properties, properties.data())};

            // VK_ERROR_OUT_OF_HOST_MEMORY
            // VK_ERROR_OUT_OF_DEVICE_MEMORY
            // VK_ERROR_LAYER_NOT_PRESENT
            assert_result_is_ok(result);

            return properties;
        }

    }  // Anonymous namespace


    ExtensionProperties Instance::extension_properties()
    {
        return vulkan::extension_properties(nullptr);
    }


    ExtensionProperties Instance::extension_properties(std::string const& layer_name)
    {
        return vulkan::extension_properties(layer_name.c_str());
    }


    bool Instance::extension_available(ExtensionProperties const& properties, std::string const& name)
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


    Instance::LayerProperties Instance::layer_properties()
    {
        std::uint32_t nr_properties{0};

        ::vkEnumerateInstanceLayerProperties(&nr_properties, nullptr);

        LayerProperties properties(nr_properties);

        VkResult result{::vkEnumerateInstanceLayerProperties(&nr_properties, properties.data())};

        // VK_ERROR_OUT_OF_HOST_MEMORY
        // VK_ERROR_OUT_OF_DEVICE_MEMORY
        assert_result_is_ok(result);

        return properties;
    }


    bool Instance::layer_available(LayerProperties const& properties, std::string const& name)
    {
        bool is_available{false};

        for (auto const& p : properties)
        {
            if (std::strcmp(p.layerName, name.c_str()) == 0)
            {
                is_available = true;
                break;
            }
        }

        return is_available;
    }


    Instance::CreateInfo::operator VkInstanceCreateInfo const*() const
    {
        return &_create_info;
    }


    std::tuple<std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t> Instance::version()
    {
        std::uint32_t packed_version;

        VkResult result{::vkEnumerateInstanceVersion(&packed_version)};

        // VK_ERROR_OUT_OF_HOST_MEMORY
        assert_result_is_ok(result);

        std::uint32_t const variant = VK_API_VERSION_VARIANT(packed_version);
        std::uint32_t const major = VK_API_VERSION_MAJOR(packed_version);
        std::uint32_t const minor = VK_API_VERSION_MINOR(packed_version);
        std::uint32_t const patch = VK_API_VERSION_PATCH(packed_version);

        return {variant, major, minor, patch};
    }


    /*!
        @brief      Initialize the Vulkan library
        @param      .
        @return     .
        @exception  .

    */
    Instance::Instance(CreateInfo const& create_info):

        _instance{}

    {
        VkResult result = ::vkCreateInstance(create_info, nullptr, &_instance);

        // VK_ERROR_OUT_OF_HOST_MEMORY
        // VK_ERROR_OUT_OF_DEVICE_MEMORY
        // VK_ERROR_INITIALIZATION_FAILED
        // VK_ERROR_LAYER_NOT_PRESENT
        // VK_ERROR_EXTENSION_NOT_PRESENT
        // VK_ERROR_INCOMPATIBLE_DRIVER
        assert_result_is_ok(result);
    }


    Instance::~Instance()
    {
        ::vkDestroyInstance(_instance, nullptr);
    }


    Instance::operator VkInstance() const
    {
        return _instance;
    }


    PhysicalDevices Instance::physical_devices() const
    {
        std::uint32_t nr_devices;
        ::vkEnumeratePhysicalDevices(_instance, &nr_devices, nullptr);

        std::vector<VkPhysicalDevice> devices1(nr_devices);
        ::vkEnumeratePhysicalDevices(_instance, &nr_devices, devices1.data());

        PhysicalDevices devices2(nr_devices);
        std::move(devices1.begin(), devices1.end(), devices2.begin());

        return devices2;
    }

}  // namespace lue::vulkan

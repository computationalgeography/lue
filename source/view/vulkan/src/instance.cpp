#include "lue/vulkan/instance.hpp"
#include "lue/vulkan/error.hpp"
#include <algorithm>
#include <cstring>


namespace lue::vulkan {

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


    Instance::Instance():

        _instance{}

    {
        assert(!is_valid());
    }


    /*!
        @brief      Initialize the Vulkan library
        @exception  std::runtime_error In case the instance cannot be created

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

        assert(is_valid());
    }


    Instance::Instance(Instance&& other):

        _instance{std::move(other._instance)}

    {
        other._instance = VkInstance{};

        assert(!other.is_valid());
    }


    Instance::~Instance()
    {
        if (is_valid())
        {
            ::vkDestroyInstance(_instance, nullptr);
            _instance = VkInstance{};
        }

        assert(!is_valid());
    }


    Instance& Instance::operator=(Instance&& other)
    {
        if (is_valid())
        {
            ::vkDestroyInstance(_instance, nullptr);
        }

        _instance = std::move(other._instance);

        other._instance = VkInstance{};

        assert(!other.is_valid());

        return *this;
    }


    bool Instance::is_valid() const
    {
        return _instance != VK_NULL_HANDLE;
    }


    /*!
        @warning    Do not use the returned pointer after this instance has gone out of scope
    */
    Instance::operator VkInstance()
    {
        assert(is_valid());

        return _instance;
    }


    PhysicalDevices Instance::physical_devices() const
    {
        assert(is_valid());

        std::uint32_t nr_devices;
        ::vkEnumeratePhysicalDevices(_instance, &nr_devices, nullptr);

        std::vector<VkPhysicalDevice> devices1(nr_devices);
        ::vkEnumeratePhysicalDevices(_instance, &nr_devices, devices1.data());

        PhysicalDevices devices2(nr_devices);
        std::move(devices1.begin(), devices1.end(), devices2.begin());

        return devices2;
    }

}  // namespace lue::vulkan

#include "lue/vulkan/instance.hpp"
#include "lue/vulkan/error.hpp"


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


    Instance::operator VkInstance const*() const
    {
        return &_instance;
    }


    // Instance::operator VkInstance const&() const
    // {
    //     return _instance;
    // }


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

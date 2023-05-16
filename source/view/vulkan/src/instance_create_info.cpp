#include "lue/vulkan/instance_create_info.hpp"


namespace lue::vulkan {

    InstanceCreateInfo::InstanceCreateInfo(
        VkInstanceCreateFlags const flags,
        ApplicationInfo&& application_info,
        Names const& enabled_layer_names,
        Names const& enabled_extension_names):

        _application_info{std::move(application_info)},
        _instance_create_info{}

    {
        _instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        _instance_create_info.flags = flags;
        _instance_create_info.pApplicationInfo = _application_info;

        _instance_create_info.enabledLayerCount = enabled_layer_names.size();
        _instance_create_info.ppEnabledLayerNames = enabled_layer_names.data();

        _instance_create_info.enabledExtensionCount = enabled_extension_names.size();
        _instance_create_info.ppEnabledExtensionNames = enabled_extension_names.data();
    }


    InstanceCreateInfo::InstanceCreateInfo(
        ApplicationInfo&& application_info,
        Names const& enabled_layer_names,
        Names const& enabled_extension_names):

        InstanceCreateInfo{
            VkInstanceCreateFlags{},
            std::move(application_info),
            enabled_layer_names,
            enabled_extension_names}

    {
    }


    InstanceCreateInfo::InstanceCreateInfo(ApplicationInfo&& application_info):

        InstanceCreateInfo{VkInstanceCreateFlags{}, std::move(application_info), Names{}, Names{}}

    {
    }


    InstanceCreateInfo::~InstanceCreateInfo()
    {
    }


    InstanceCreateInfo::operator VkInstanceCreateInfo const*()
    {
        return &_instance_create_info;
    }

}  // namespace lue::vulkan

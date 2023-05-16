#pragma once
#include <string>
#include <vector>
#include <vulkan/vulkan.h>


namespace lue::vulkan {

    using InstanceExtensionProperties = std::vector<VkExtensionProperties>;

    InstanceExtensionProperties instance_extension_properties();

    bool instance_extension_available(InstanceExtensionProperties const& properties, std::string const& name);

}  // namespace lue::vulkan

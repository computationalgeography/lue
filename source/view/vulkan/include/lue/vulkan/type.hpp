#pragma once
#include <vulkan/vulkan.h>
#include <vector>


namespace lue::vulkan {

    using ExtensionProperties = std::vector<VkExtensionProperties>;

    using Names = std::vector<char const*>;

}  // namespace lue::vulkan

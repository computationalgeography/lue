#include "lue/vulkan/extension.hpp"
#include <cinttypes>
#include <cstring>


namespace lue::vulkan {

    InstanceExtensionProperties instance_extension_properties()
    {
        std::uint32_t nr_extensions{0};

        ::vkEnumerateInstanceExtensionProperties(nullptr, &nr_extensions, nullptr);

        InstanceExtensionProperties properties(nr_extensions);

        ::vkEnumerateInstanceExtensionProperties(nullptr, &nr_extensions, properties.data());

        return properties;
    }


    bool instance_extension_available(InstanceExtensionProperties const& properties, std::string const& name)
    {
        for (auto const& p : properties)
        {
            if (std::strcmp(p.extensionName, name.c_str()) == 0)
            {
                return true;
            }
        }

        return false;
    }

}  // namespace lue::vulkan

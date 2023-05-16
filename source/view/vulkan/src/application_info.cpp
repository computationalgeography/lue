#include "lue/vulkan/application_info.hpp"


namespace lue::vulkan {

    ApplicationInfo::ApplicationInfo(std::string const& application_name, std::string const& engine_name):

        _application_info{}

    {
        _application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        _application_info.pApplicationName = application_name.c_str();
        _application_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        _application_info.pEngineName = engine_name.c_str();
        _application_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        _application_info.apiVersion = VK_API_VERSION_1_0;
    }


    ApplicationInfo::~ApplicationInfo()
    {
    }


    ApplicationInfo::operator VkApplicationInfo const*()
    {
        return &_application_info;
    }

}  // namespace lue::vulkan

#include "lue/vulkan/application_info.hpp"


namespace lue::vulkan {

    ApplicationInfo::ApplicationInfo(
        std::string const& application_name,
        std::tuple<std::uint32_t, std::uint32_t, std::uint32_t> const& version,
        std::uint32_t const api_version):

        _application_name{application_name},
        _engine_name{},
        _application_info{}

    {
        _application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;

        if (!_application_name.empty())
        {
            // The member variable keeps the character buffer alive
            _application_info.pApplicationName = _application_name.c_str();
        }

        _application_info.applicationVersion =
            VK_MAKE_VERSION(std::get<0>(version), std::get<1>(version), std::get<2>(version));

        if (!_engine_name.empty())
        {
            // The member variable keeps the character buffer alive
            _application_info.pEngineName = _engine_name.c_str();
        }

        // _application_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);

        // The highest version of Vulkan that the application is designed to use
        _application_info.apiVersion = api_version;
    }


    ApplicationInfo::operator VkApplicationInfo const*() const
    {
        return &_application_info;
    }

}  // namespace lue::vulkan

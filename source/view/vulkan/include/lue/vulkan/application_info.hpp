#pragma once
#include "lue/vulkan/type.hpp"
#include <string>
#include <tuple>


namespace lue::vulkan {

    class ApplicationInfo
    {

        public:

            ApplicationInfo(
                std::string const& application_name,
                std::tuple<std::uint32_t, std::uint32_t, std::uint32_t> const& version,
                std::uint32_t const api_version);

            ApplicationInfo(ApplicationInfo const&) = delete;

            ApplicationInfo(ApplicationInfo&&) = default;

            ~ApplicationInfo() = default;

            ApplicationInfo& operator=(ApplicationInfo const&) = delete;

            ApplicationInfo& operator=(ApplicationInfo&&) = default;

            operator VkApplicationInfo const*() const;

        private:

            std::string _application_name;

            std::string _engine_name;

            static_assert(!std::is_pointer_v<VkApplicationInfo>);

            VkApplicationInfo _application_info;
    };

}  // namespace lue::vulkan

#pragma once
#include "lue/vulkan/queue_family.hpp"
#include "lue/vulkan/surface.hpp"
#include "lue/vulkan/type.hpp"
#include <string>


namespace lue::vulkan {

    class PhysicalDevice
    {

        public:

            class Properties
            {

                public:

                    Properties(VkPhysicalDeviceProperties&& properties);

                    Properties(Properties const&) = delete;

                    Properties(Properties&&) = default;

                    ~Properties() = default;

                    Properties& operator=(Properties const&) = delete;

                    Properties& operator=(Properties&&) = default;

                    std::string device_name() const;

                    VkPhysicalDeviceType device_type() const;

                private:

                    static_assert(!std::is_pointer_v<VkPhysicalDeviceProperties>);

                    VkPhysicalDeviceProperties _properties;
            };


            /*!
                @brief      Class for querying for optional features
                @param      .
                @return     .
                @exception  .
            */
            class Features
            {

                public:

                    Features() = default;

                    Features(VkPhysicalDeviceFeatures&& properties);

                    Features(Features const&) = delete;

                    Features(Features&&) = default;

                    ~Features() = default;

                    Features& operator=(Features const&) = delete;

                    Features& operator=(Features&&) = default;

                    operator VkPhysicalDeviceFeatures const*() const;

                    bool has_geometry_shader() const;

                private:

                    static_assert(!std::is_pointer_v<VkPhysicalDeviceFeatures>);

                    VkPhysicalDeviceFeatures _features;
            };


            class SurfaceProperties
            {

                public:

                    using Formats = std::vector<VkSurfaceFormatKHR>;

                    using PresentModes = std::vector<VkPresentModeKHR>;

                    SurfaceProperties() = default;

                    SurfaceProperties(
                        VkSurfaceCapabilitiesKHR&& capabilities,
                        Formats&& formats,
                        PresentModes&& present_modes);

                    VkSurfaceCapabilitiesKHR const& capabilities() const;

                    Formats const& formats() const;

                    PresentModes const& present_modes() const;

                private:

                    static_assert(!std::is_pointer_v<VkSurfaceCapabilitiesKHR>);

                    VkSurfaceCapabilitiesKHR _capabilities;

                    static_assert(!std::is_pointer_v<VkSurfaceFormatKHR>);

                    Formats _formats;

                    static_assert(!std::is_pointer_v<VkPresentModeKHR>);

                    PresentModes _present_modes;
            };


            PhysicalDevice();

            PhysicalDevice(VkPhysicalDevice physical_device);

            PhysicalDevice(PhysicalDevice const&) = delete;

            PhysicalDevice(PhysicalDevice&& other);

            ~PhysicalDevice() = default;

            PhysicalDevice& operator=(PhysicalDevice const&) = delete;

            PhysicalDevice& operator=(PhysicalDevice&& other);

            bool is_valid() const;

            operator VkPhysicalDevice();

            Properties properties() const;

            Features features() const;

            QueueFamilyProperties queue_family_properties() const;

            bool extensions_available(Names const& names) const;

            bool has_surface_support(QueueFamily const& queue_family, Surface& surface) const;

            SurfaceProperties surface_properties(Surface& surface) const;

        private:

            ExtensionProperties extension_properties() const;

            static bool extension_available(ExtensionProperties const& properties, std::string const& name);

            static_assert(std::is_pointer_v<VkPhysicalDevice>);

            VkPhysicalDevice _physical_device;
    };

}  // namespace lue::vulkan

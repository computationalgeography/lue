#pragma once
#include <vulkan/vulkan.h>
#include <string>
#include <type_traits>


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

                    VkPhysicalDeviceProperties _properties;
            };


            class Features
            {

                public:

                    Features(VkPhysicalDeviceFeatures&& properties);

                    Features(Features const&) = delete;

                    Features(Features&&) = default;

                    ~Features() = default;

                    Features& operator=(Features const&) = delete;

                    Features& operator=(Features&&) = default;

                    bool has_geometry_shader() const;

                private:

                    VkPhysicalDeviceFeatures _features;
            };


            class QueueFamily
            {

                public:

                    class Properties
                    {

                        public:

                            Properties(VkQueueFamilyProperties&& properties);

                            bool graphics() const;

                        private:

                            static_assert(!std::is_pointer_v<VkQueueFamilyProperties>);

                            VkQueueFamilyProperties _properties;
                    };
            };


            PhysicalDevice() = default;

            PhysicalDevice(VkPhysicalDevice physical_device);

            PhysicalDevice(PhysicalDevice const&) = delete;

            PhysicalDevice(PhysicalDevice&&) = default;

            ~PhysicalDevice() = default;

            PhysicalDevice& operator=(PhysicalDevice const&) = delete;

            PhysicalDevice& operator=(PhysicalDevice&&) = default;

            Properties properties() const;

            Features features() const;

        private:

            static_assert(std::is_pointer_v<VkPhysicalDevice>);

            VkPhysicalDevice _physical_device;
    };

}  // namespace lue::vulkan

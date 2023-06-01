#pragma once
#include <vulkan/vulkan.h>
#include <string>
#include <type_traits>
#include <vector>


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


            class QueueFamily
            {

                public:

                    class Properties
                    {

                        public:

                            Properties();

                            Properties(VkQueueFamilyProperties&& properties);

                            Properties(Properties const&) = default;

                            Properties(Properties&&) = default;

                            ~Properties() = default;

                            Properties& operator=(Properties const&) = delete;

                            Properties& operator=(Properties&&) = default;

                            bool graphics() const;

                        private:

                            static_assert(!std::is_pointer_v<VkQueueFamilyProperties>);

                            VkQueueFamilyProperties _properties;
                    };


                    QueueFamily();

                    explicit QueueFamily(std::uint32_t const idx);

                    std::uint32_t idx() const;

                private:

                    std::uint32_t _idx;
            };


            using QueueFamilyProperties = std::vector<QueueFamily::Properties>;


            PhysicalDevice() = default;

            PhysicalDevice(VkPhysicalDevice physical_device);

            PhysicalDevice(PhysicalDevice const&) = delete;

            PhysicalDevice(PhysicalDevice&&) = default;

            ~PhysicalDevice() = default;

            PhysicalDevice& operator=(PhysicalDevice const&) = delete;

            PhysicalDevice& operator=(PhysicalDevice&&) = default;

            operator VkPhysicalDevice() const;

            Properties properties() const;

            Features features() const;

            QueueFamilyProperties queue_family_properties() const;

        private:

            static_assert(std::is_pointer_v<VkPhysicalDevice>);

            VkPhysicalDevice _physical_device;
    };

}  // namespace lue::vulkan

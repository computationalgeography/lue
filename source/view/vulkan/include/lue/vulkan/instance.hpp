#pragma once
#include "lue/vulkan/application_info.hpp"
#include "lue/vulkan/physical_device.hpp"
#include <cassert>
#include <tuple>


namespace lue::vulkan {

    using PhysicalDevices = std::vector<PhysicalDevice>;


    /*!
        @brief      Class for all per-application state

        This class wraps a VkInstance, which is a pointer to the actual instance object. Copying
        Instance instances is not supported, because the one and only instance is responsible
        for destroying it. The wrapped VkInstance can be obtained. This will happen automatically
        due to the conversion operator. The one and only Instance instance must outlive all
        copies and pointers to its wrapped VkInstance instance.

        Pattern:
        - Create Instance instance
        - Use instance in other API functions
        - Once all is really done, allow the Instance instance to go out of scope
    */
    class Instance
    {

        public:

            using LayerProperties = std::vector<VkLayerProperties>;


            /*!
                @brief      .
                @param      .
                @return     .
                @exception  .
                @sa         https://registry.khronos.org/vulkan/specs/1.3/html/chap4.html#VkInstance
            */
            class CreateInfo
            {

                    // TODO Support adding a debug print callback

                public:

                    CreateInfo(
                        VkInstanceCreateFlags const flags,
                        ApplicationInfo&& application_info,
                        Names const& enabled_layer_names,
                        Names const& enabled_extension_names);

                    CreateInfo(
                        ApplicationInfo&& application_info,
                        Names const& enabled_layer_names,
                        Names const& enabled_extension_names);

                    CreateInfo(ApplicationInfo&& application_info);

                    CreateInfo(CreateInfo const&) = delete;

                    CreateInfo(CreateInfo&&) = default;

                    ~CreateInfo() = default;

                    CreateInfo& operator=(CreateInfo const&) = delete;

                    CreateInfo& operator=(CreateInfo&&) = default;

                    operator VkInstanceCreateInfo const*() const;

                private:

                    ApplicationInfo _application_info;

                    VkInstanceCreateInfo _create_info;
            };

            static ExtensionProperties extension_properties();

            static ExtensionProperties extension_properties(std::string const& layer_name);

            static bool extension_available(ExtensionProperties const& properties, std::string const& name);

            static LayerProperties layer_properties();

            static bool layer_available(LayerProperties const& properties, std::string const& name);

            static std::tuple<std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t> version();

            Instance() = default;

            Instance(CreateInfo const& create_info);

            Instance(Instance const& other) = delete;

            Instance(Instance&& other);

            ~Instance();

            Instance& operator=(Instance const&) = delete;

            Instance& operator=(Instance&& other);

            operator bool() const;

            operator VkInstance() const;

            PhysicalDevices physical_devices() const;

            template<typename FP>
            FP function_pointer(char const* name) const
            {
                auto fp = (FP)::vkGetInstanceProcAddr(_instance, name);
                assert(fp != nullptr);

                return fp;
            }

        private:

            static_assert(std::is_pointer_v<VkInstance>);

            //! Handle to underlying instance object
            VkInstance _instance;
    };

}  // namespace lue::vulkan

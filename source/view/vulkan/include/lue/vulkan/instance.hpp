#pragma once
#include "lue/vulkan/application_info.hpp"
#include "lue/vulkan/physical_device.hpp"
#include "lue/vulkan/structure.hpp"
#include <cassert>
#include <tuple>


namespace lue::vulkan {

    using PhysicalDevices = std::vector<PhysicalDevice>;


    /*!
        @brief      Class for all per-application state

        This class wraps a `VkInstance`, which is a pointer to the actual instance object. Copying
        Instance instances is not supported, because the one and only instance is responsible
        for destroying it. The wrapped `VkInstance` can be obtained. This will happen automatically
        due to the conversion operator. The one and only Instance instance must outlive all
        copies of and pointers to its wrapped `VkInstance` instance.

        Pattern:
        - Create Instance instance
        - Use instance in other API functions
        - Once all is really done, allow the Instance instance to go out of scope
    */
    class Instance
    {

        public:

            using LayerProperties = std::vector<VkLayerProperties>;

            using CreateInfo = Structure<VkInstanceCreateInfo, VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO>;


            static ExtensionProperties extension_properties();

            static ExtensionProperties extension_properties(std::string const& layer_name);

            static bool extension_available(ExtensionProperties const& properties, std::string const& name);

            static LayerProperties layer_properties();

            static bool layer_available(LayerProperties const& properties, std::string const& name);

            static std::tuple<std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t> version();

            Instance();

            Instance(CreateInfo const& create_info);

            Instance(Instance const& other) = delete;

            Instance(Instance&& other);

            ~Instance();

            Instance& operator=(Instance const&) = delete;

            Instance& operator=(Instance&& other);

            bool is_valid() const;

            operator VkInstance();

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
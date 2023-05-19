#pragma once
#include "lue/vulkan/application_info.hpp"
#include "lue/vulkan/physical_device.hpp"
#include <cassert>
#include <tuple>


namespace lue::vulkan {

    using PhysicalDevices = std::vector<PhysicalDevice>;


    /*!
        @brief      Class for all per-application state
        @param      .
        @return     .
        @exception  .
    */
    class Instance
    {

        public:

            /*!
                @brief      .
                @param      .
                @return     .
                @exception  .
                @sa         https://registry.khronos.org/vulkan/specs/1.3/html/chap4.html#VkInstance
            */
            class CreateInfo
            {

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


            static std::tuple<std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t> version();

            Instance(CreateInfo const& create_info);

            Instance(Instance const&) = delete;

            Instance(Instance&&) = default;

            ~Instance();

            Instance& operator=(Instance const&) = delete;

            Instance& operator=(Instance&&) = default;

            operator VkInstance const*() const;

            // operator VkInstance const&() const;

            PhysicalDevices physical_devices() const;

            template<typename FP>
            FP function_pointer(char const* name) const
            {
                auto fp = (FP)::vkGetInstanceProcAddr(_instance, name);
                assert(fp != nullptr);

                return fp;
            }

        private:

            VkInstance _instance;
    };

}  // namespace lue::vulkan

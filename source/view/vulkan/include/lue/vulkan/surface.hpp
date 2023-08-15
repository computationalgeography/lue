#pragma once
#include <vulkan/vulkan.h>
#include <type_traits>


namespace lue::vulkan {

    class Surface
    {
        public:

            Surface();

            Surface(VkInstance instance, VkSurfaceKHR surface);

            Surface(Surface const&) = delete;

            Surface(Surface&& other);

            ~Surface();

            Surface& operator=(Surface const&) = delete;

            Surface& operator=(Surface&& other);

            bool is_valid() const;

            operator VkSurfaceKHR();

        private:

            static_assert(std::is_pointer_v<VkInstance>);

            VkInstance _instance;

            static_assert(std::is_pointer_v<VkSurfaceKHR>);

            VkSurfaceKHR _surface;
    };

}  // namespace lue::vulkan

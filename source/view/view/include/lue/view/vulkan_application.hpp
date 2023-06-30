#pragma once
#include "lue/view/application.hpp"
#include "lue/vulkan.hpp"
#include <memory>


namespace lue::glfw {

    class Library;
    class Monitor;
    class Window;

}  // namespace lue::glfw


namespace lue::view {

    class VulkanApplication: public Application
    {

        public:

            VulkanApplication(std::vector<std::string> const& arguments);

            ~VulkanApplication();

        protected:

            int run_implementation() final;

        private:

            vulkan::QueueFamilies find_queue_families(vulkan::PhysicalDevice const& physical_device) const;

            bool physical_device_is_suitable(vulkan::PhysicalDevice const& device) const;

            void init_window();

            void init_vulkan();

            void create_instance();

            void setup_debug_messenger();

            void create_surface();

            void pick_physical_device();

            void create_logical_device();

            void create_swapchain();

            std::unique_ptr<glfw::Library> _library;

            std::unique_ptr<glfw::Monitor> _monitor;

            std::unique_ptr<glfw::Window> _window;

            vulkan::Instance _instance;

#ifndef NDEBUG
            vulkan::DebugReportCallback _debug_callback;
#endif

            vulkan::Surface _surface;

            vulkan::PhysicalDevice _physical_device;

            vulkan::Device _device;

            vulkan::Device::Queue _graphics_queue;

            vulkan::Device::Queue _present_queue;
    };

}  // namespace lue::view

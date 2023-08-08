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

            vulkan::Names required_extension_names() const;

            vulkan::Names required_layer_names() const;

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

            void create_image_views();

            void create_render_pass();

            void create_graphics_pipeline();

            void create_framebuffers();

            std::unique_ptr<glfw::Library> _library;

            std::unique_ptr<glfw::Monitor> _monitor;

            std::unique_ptr<glfw::Window> _window;

            bool _enable_validation_layers;

            vulkan::Instance _instance;

            vulkan::DebugReportCallback _debug_callback;

            vulkan::Surface _surface;

            vulkan::PhysicalDevice _physical_device;

            vulkan::Device _device;

            vulkan::Device::Queue _graphics_queue;

            vulkan::Device::Queue _present_queue;

            vulkan::Swapchain _swapchain;

            vulkan::Swapchain::Images _swapchain_images;

            VkFormat _image_format;

            VkExtent2D _image_extent;

            vulkan::ImageViews _swapchain_image_views;

            vulkan::RenderPass _render_pass;

            vulkan::PipelineLayout _pipeline_layout;

            vulkan::Pipeline _graphics_pipeline;

            vulkan::Framebuffers _framebuffers;
    };

}  // namespace lue::view

#pragma once
#include "lue/view/application.hpp"
#include "lue/vulkan.hpp"
#include <memory>


namespace lue::glfw {

    class Library;
    class Monitor;
    class Window;

}  // namespace lue::glfw


namespace lue::imgui::glfw {

    class VulkanBinding;

}  // namespace lue::imgui::glfw


struct GLFWwindow;


namespace lue::view {

    static void framebuffer_resize_callback(GLFWwindow* window, int width, int height);


    class VulkanApplication: public Application
    {

        public:

            friend void framebuffer_resize_callback(GLFWwindow*, int, int);

            VulkanApplication(std::vector<std::string> const& arguments);

            ~VulkanApplication();

            // void framebuffer_is_resized();

        protected:

            int run_implementation() final;

        private:

            vulkan::Names required_extension_names() const;

            vulkan::Names required_layer_names() const;

            vulkan::QueueFamilies find_queue_families(vulkan::PhysicalDevice const& physical_device);

            bool physical_device_is_suitable(vulkan::PhysicalDevice const& device);

            void init_window();

            void init_vulkan();

            void init_imgui();

            void create_instance();

            void setup_debug_messenger();

            void create_surface();

            void pick_physical_device();

            void create_logical_device();

            void create_swapchain();

            void recreate_swapchain();

            void create_image_views();

            void create_render_pass();

            void create_graphics_pipeline();

            void create_framebuffers();

            void create_command_pool();

            void create_command_buffers();

            void create_sync_objects();

            void record_command_buffer(vulkan::CommandBuffer& command_buffer, std::uint32_t image_idx);

            void main_loop();

            void draw_frame();

            std::unique_ptr<glfw::Library> _library;

            std::unique_ptr<glfw::Monitor> _monitor;

            std::unique_ptr<glfw::Window> _window;

            std::unique_ptr<imgui::glfw::VulkanBinding> _binding;

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

            vulkan::CommandPool _command_pool;

            vulkan::Device::CommandBuffers _command_buffers;

            vulkan::Device::Semaphores _image_available_semaphores;

            vulkan::Device::Semaphores _render_finished_semaphores;

            vulkan::Device::Fences _in_flight_fences;

            std::uint32_t _current_frame;

            bool _framebuffer_resized;
    };

}  // namespace lue::view

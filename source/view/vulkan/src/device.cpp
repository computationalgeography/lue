#include "lue/vulkan/device.hpp"
#include "lue/vulkan/error.hpp"
#include <algorithm>
#include <cassert>


namespace lue::vulkan {

    Device::Device():

        _device{}

    {
        assert(!is_valid());
    }


    Device::Device(PhysicalDevice& physical_device, CreateInfo const& create_info):

        _device{}

    {
        VkResult result = vkCreateDevice(physical_device, create_info, nullptr, &_device);

        // VK_ERROR_OUT_OF_HOST_MEMORY
        // VK_ERROR_OUT_OF_DEVICE_MEMORY
        // VK_ERROR_INITIALIZATION_FAILED
        // VK_ERROR_EXTENSION_NOT_PRESENT
        // VK_ERROR_FEATURE_NOT_PRESENT
        // VK_ERROR_TOO_MANY_OBJECTS
        // VK_ERROR_DEVICE_LOST
        assert_result_is_ok(result);

        assert(is_valid());
    }


    Device::Device(Device&& other):

        _device{std::move(other._device)}

    {
        other._device = VkDevice{};

        assert(!other.is_valid());
    }


    Device::~Device()
    {
        if (is_valid())
        {
            vkDestroyDevice(_device, nullptr);

            _device = VkDevice{};
        }

        assert(!is_valid());
    }


    Device& Device::operator=(Device&& other)
    {
        _device = std::move(other._device);

        other._device = VkDevice{};

        assert(!other.is_valid());

        return *this;
    }


    bool Device::is_valid() const
    {
        return _device != VK_NULL_HANDLE;
    }


    /*!
        @warning    Do not use the returned pointer after this instance has gone out of scope
    */
    Device::operator VkDevice()
    {
        assert(is_valid());

        return _device;
    }


    void Device::wait_idle() const
    {
        VkResult result = vkDeviceWaitIdle(_device);

        assert_result_is_ok(result);
    }


    Queue Device::queue(QueueFamily const& queue_family) const
    {
        assert(is_valid());

        VkQueue queue;

        vkGetDeviceQueue(_device, queue_family, 0, &queue);

        return queue;
    }


    Swapchain Device::create_swapchain(Swapchain::CreateInfo const& create_info) const
    {
        assert(is_valid());

        VkSwapchainKHR swapchain;

        VkResult result = vkCreateSwapchainKHR(_device, create_info, nullptr, &swapchain);

        assert_result_is_ok(result);

        return {_device, swapchain};
    }


    ImageView Device::create_image_view(ImageView::CreateInfo const& create_info) const
    {
        assert(is_valid());

        VkImageView image_view;

        VkResult result{vkCreateImageView(_device, create_info, nullptr, &image_view)};

        assert_result_is_ok(result);

        return {_device, image_view};
    }


    ShaderModule Device::create_shader_module(ShaderModule::CreateInfo const& create_info) const
    {
        assert(is_valid());

        VkShaderModule shader_module;

        VkResult result{vkCreateShaderModule(_device, create_info, nullptr, &shader_module)};

        assert_result_is_ok(result);

        return {_device, shader_module};
    }


    PipelineLayout Device::create_pipeline_layout(PipelineLayout::CreateInfo const& create_info) const
    {
        assert(is_valid());

        VkPipelineLayout pipeline_layout;

        VkResult result{vkCreatePipelineLayout(_device, create_info, nullptr, &pipeline_layout)};

        assert_result_is_ok(result);

        return {_device, pipeline_layout};
    }


    RenderPass Device::create_render_pass(RenderPass::CreateInfo const& create_info) const
    {
        assert(is_valid());

        VkRenderPass render_pass;

        VkResult result{vkCreateRenderPass(_device, create_info, nullptr, &render_pass)};

        assert_result_is_ok(result);

        return {_device, render_pass};
    }


    Pipeline Device::create_graphics_pipeline(Pipeline::GraphicsPipelineCreateInfo const& create_info) const
    {
        assert(is_valid());

        VkPipeline graphics_pipeline;

        VkResult result{
            vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, create_info, nullptr, &graphics_pipeline)};

        assert_result_is_ok(result);

        return {_device, graphics_pipeline};
    }


    Framebuffer Device::create_framebuffer(Framebuffer::CreateInfo const& create_info) const
    {
        assert(is_valid());

        VkFramebuffer framebuffer;

        VkResult result{vkCreateFramebuffer(_device, create_info, nullptr, &framebuffer)};

        assert_result_is_ok(result);

        return {_device, framebuffer};
    }


    CommandPool Device::create_command_pool(CommandPool::CreateInfo const& create_info) const
    {
        assert(is_valid());

        VkCommandPool command_pool;

        VkResult result{vkCreateCommandPool(_device, create_info, nullptr, &command_pool)};

        assert_result_is_ok(result);

        return {_device, command_pool};
    }


    Buffer Device::create_buffer(Buffer::CreateInfo const& create_info) const
    {
        assert(is_valid());

        VkBuffer buffer;

        VkResult result{vkCreateBuffer(_device, create_info, nullptr, &buffer)};

        assert_result_is_ok(result);

        return {_device, buffer};
    }


    CommandBuffers Device::allocate_command_buffers(CommandBuffer::AllocateInfo const& allocate_info) const
    {
        assert(is_valid());

        std::uint32_t const nr_command_buffers = (*allocate_info).commandBufferCount;

        std::vector<VkCommandBuffer> command_buffers1(nr_command_buffers);

        VkResult result{vkAllocateCommandBuffers(_device, allocate_info, command_buffers1.data())};

        assert_result_is_ok(result);

        CommandBuffers command_buffers2(nr_command_buffers);

        std::transform(
            command_buffers1.begin(),
            command_buffers1.end(),
            command_buffers2.begin(),
            [this](VkCommandBuffer command_buffer) {
                return CommandBuffer{_device, command_buffer};
            });

        return command_buffers2;
    }


    Semaphore Device::create_semaphore(Semaphore::CreateInfo const& create_info) const
    {
        assert(is_valid());

        VkSemaphore semaphore;

        VkResult result{vkCreateSemaphore(_device, create_info, nullptr, &semaphore)};

        assert_result_is_ok(result);

        return {_device, semaphore};
    }


    Device::Semaphores Device::create_semaphores(
        std::uint32_t const nr_semaphores, Semaphore::CreateInfo const& create_info) const
    {
        Semaphores semaphores(nr_semaphores);

        std::generate(
            semaphores.begin(),
            semaphores.end(),
            [this, create_info = std::ref(create_info)]() { return create_semaphore(create_info); });

        return semaphores;
    }


    Fence Device::create_fence(Fence::CreateInfo const& create_info) const
    {
        assert(is_valid());

        VkFence fence;

        VkResult result{vkCreateFence(_device, create_info, nullptr, &fence)};

        assert_result_is_ok(result);

        return {_device, fence};
    }


    Device::Fences Device::create_fences(
        std::uint32_t const nr_fences, Fence::CreateInfo const& create_info) const
    {
        Fences fences(nr_fences);

        std::generate(
            fences.begin(),
            fences.end(),
            [this, create_info = std::ref(create_info)]() { return create_fence(create_info); });

        return fences;
    }


    VkResult Device::wait(Fence& fence)
    {
        return vkWaitForFences(_device, 1, fence, VK_TRUE, UINT64_MAX);
    }


    VkResult Device::reset(Fence& fence)
    {
        return vkResetFences(_device, 1, fence);
    }


    std::tuple<VkResult, std::uint32_t> Device::acquire_next_image(Swapchain& swapchain, Semaphore& semaphore)
    {
        std::uint32_t image_idx;

        VkResult result =
            vkAcquireNextImageKHR(_device, swapchain, UINT64_MAX, semaphore, VK_NULL_HANDLE, &image_idx);

        return {result, image_idx};
    }


    Memory Device::allocate_memory(Memory::AllocateInfo const& allocate_info)
    {
        VkDeviceMemory memory;

        VkResult result = vkAllocateMemory(_device, allocate_info, nullptr, &memory);

        assert_result_is_ok(result);

        return {_device, memory};
    }


    void Device::map_memory(
        Memory& memory,
        VkDeviceSize const offset,
        VkDeviceSize const size,
        VkMemoryMapFlags const flags,
        void** data)
    {
        VkResult result = vkMapMemory(_device, memory, offset, size, flags, data);

        assert_result_is_ok(result);
    }


    void Device::unmap_memory(Memory& memory)
    {
        vkUnmapMemory(_device, memory);
    }

}  // namespace lue::vulkan

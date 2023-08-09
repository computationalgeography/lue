#include "lue/vulkan/device.hpp"
#include "lue/vulkan/error.hpp"
#include <algorithm>
#include <cassert>


namespace lue::vulkan {

    /*!
        @brief      Constructor for information about a single queue
        @sa         https://registry.khronos.org/vulkan/specs/1.3/html/chap5.html#VkDeviceQueueCreateInfo
    */
    Device::QueueCreateInfo::QueueCreateInfo(QueueFamily const& queue_family):

        _create_info{},
        _queue_priorities{1.0f}

    {
        _create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        _create_info.queueFamilyIndex = queue_family;
        _create_info.queueCount = 1;
        _create_info.pQueuePriorities = _queue_priorities.data();
    }


    /*!
        @warning    Do not use the returned pointer after this instance has gone out of scope
    */
    Device::QueueCreateInfo::operator VkDeviceQueueCreateInfo const*() const
    {
        return &_create_info;
    }


    Device::CreateInfo::CreateInfo(
        std::vector<QueueCreateInfo>&& queue_create_infos, Names&& extension_names):

        CreateInfo{std::move(queue_create_infos), PhysicalDevice::Features{}, std::move(extension_names)}

    {
    }


    Device::CreateInfo::CreateInfo(
        std::vector<QueueCreateInfo>&& queue_create_infos,
        PhysicalDevice::Features&& enabled_features,
        Names&& extension_names):

        _queue_create_infos(queue_create_infos.size()),
        _enabled_features{std::move(enabled_features)},
        _extension_names{std::move(extension_names)},
        _create_info{}

    {
        std::transform(
            queue_create_infos.begin(),
            queue_create_infos.end(),
            _queue_create_infos.begin(),
            [](QueueCreateInfo const& queue_create_info) { return *queue_create_info; });

        _create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        _create_info.pQueueCreateInfos = _queue_create_infos.data();
        _create_info.queueCreateInfoCount = _queue_create_infos.size();
        _create_info.pEnabledFeatures = _enabled_features;
        _create_info.enabledExtensionCount = _extension_names.size();
        _create_info.ppEnabledExtensionNames = _extension_names.data();
    }


    /*!
        @warning    Do not use the returned pointer after this instance has gone out of scope
    */
    Device::CreateInfo::operator VkDeviceCreateInfo const*() const
    {
        return &_create_info;
    }


    Device::PresentInfo::PresentInfo():

        _present_info{}

    {
        _present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    }


    /*!
        @warning    Do not use the returned pointer after this instance has gone out of scope
    */
    Device::PresentInfo::operator VkPresentInfoKHR const*() const
    {
        return &_present_info;
    }


    VkPresentInfoKHR& Device::PresentInfo::operator*()
    {
        return _present_info;
    }


    Device::Queue::Queue():

        _queue{}

    {
        assert(!*this);
    }


    Device::Queue::Queue(VkQueue queue):

        _queue{queue}

    {
        assert(*this);
    }


    Device::Queue::Queue(Device::Queue&& other):

        _queue{std::move(other._queue)}

    {
        other._queue = VkQueue{};

        assert(!other);
    }


    Device::Queue& Device::Queue::operator=(Device::Queue&& other)
    {
        _queue = std::move(other._queue);

        other._queue = VkQueue{};

        assert(!other);

        return *this;
    }


    Device::Queue::operator bool() const
    {
        return _queue != VK_NULL_HANDLE;
    }


    /*!
        @warning    Do not use the returned pointer after this instance has gone out of scope
    */
    Device::Queue::operator VkQueue() const
    {
        assert(*this);

        return _queue;
    }


    void Device::Queue::present(PresentInfo const& present_info)
    {
        VkResult result = vkQueuePresentKHR(_queue, present_info);

        assert_result_is_ok(result);
    }


    Device::Device():

        _device{}

    {
        assert(!*this);
    }


    Device::Device(PhysicalDevice const& physical_device, CreateInfo const& create_info):

        _device{}

    {
        VkResult result = ::vkCreateDevice(physical_device, create_info, nullptr, &_device);

        // VK_ERROR_OUT_OF_HOST_MEMORY
        // VK_ERROR_OUT_OF_DEVICE_MEMORY
        // VK_ERROR_INITIALIZATION_FAILED
        // VK_ERROR_EXTENSION_NOT_PRESENT
        // VK_ERROR_FEATURE_NOT_PRESENT
        // VK_ERROR_TOO_MANY_OBJECTS
        // VK_ERROR_DEVICE_LOST
        assert_result_is_ok(result);

        assert(*this);
    }


    Device::Device(Device&& other):

        _device{std::move(other._device)}

    {
        other._device = VkDevice{};

        assert(!other);
    }


    Device::~Device()
    {
        if (*this)
        {
            ::vkDestroyDevice(_device, nullptr);
            _device = VkDevice{};
        }

        assert(!*this);
    }


    Device& Device::operator=(Device&& other)
    {
        _device = std::move(other._device);

        other._device = VkDevice{};

        assert(!other);

        return *this;
    }


    Device::operator bool() const
    {
        return _device != VK_NULL_HANDLE;
    }


    /*!
        @warning    Do not use the returned pointer after this instance has gone out of scope
    */
    Device::operator VkDevice() const
    {
        assert(*this);

        return _device;
    }


    void Device::wait_idle() const
    {
        VkResult result = vkDeviceWaitIdle(_device);

        assert_result_is_ok(result);
    }


    Device::Queue Device::queue(QueueFamily const& queue_family) const
    {
        assert(*this);

        ::VkQueue queue;

        ::vkGetDeviceQueue(_device, queue_family, 0, &queue);

        return queue;
    }


    Swapchain Device::swapchain(Swapchain::CreateInfo const& create_info) const
    {
        assert(*this);

        VkSwapchainKHR swapchain;

        VkResult result = vkCreateSwapchainKHR(_device, create_info, nullptr, &swapchain);

        assert_result_is_ok(result);

        return {_device, swapchain};
    }


    ImageView Device::image_view(ImageView::CreateInfo const& create_info) const
    {
        assert(*this);

        VkImageView image_view;

        VkResult result{vkCreateImageView(_device, create_info, nullptr, &image_view)};

        assert_result_is_ok(result);

        return {_device, image_view};
    }


    ShaderModule Device::shader_module(ShaderModule::CreateInfo const& create_info) const
    {
        assert(*this);

        VkShaderModule shader_module;

        VkResult result{vkCreateShaderModule(_device, create_info, nullptr, &shader_module)};

        assert_result_is_ok(result);

        return {_device, shader_module};
    }


    PipelineLayout Device::pipeline_layout(PipelineLayout::CreateInfo const& create_info) const
    {
        assert(*this);

        VkPipelineLayout pipeline_layout;

        VkResult result{vkCreatePipelineLayout(_device, create_info, nullptr, &pipeline_layout)};

        assert_result_is_ok(result);

        return {_device, pipeline_layout};
    }


    RenderPass Device::render_pass(RenderPass::CreateInfo const& create_info) const
    {
        assert(*this);

        VkRenderPass render_pass;

        VkResult result{vkCreateRenderPass(_device, create_info, nullptr, &render_pass)};

        assert_result_is_ok(result);

        return {_device, render_pass};
    }


    Pipeline Device::graphics_pipeline(Pipeline::GraphicsPipelineCreateInfo const& create_info) const
    {
        assert(*this);

        VkPipeline graphics_pipeline;

        VkResult result{
            vkCreateGraphicsPipelines(_device, VK_NULL_HANDLE, 1, create_info, nullptr, &graphics_pipeline)};

        assert_result_is_ok(result);

        return {_device, graphics_pipeline};
    }


    Framebuffer Device::framebuffer(Framebuffer::CreateInfo const& create_info) const
    {
        assert(*this);

        VkFramebuffer framebuffer;

        VkResult result{vkCreateFramebuffer(_device, create_info, nullptr, &framebuffer)};

        assert_result_is_ok(result);

        return {_device, framebuffer};
    }


    CommandPool Device::command_pool(CommandPool::CreateInfo const& create_info) const
    {
        assert(*this);

        VkCommandPool command_pool;

        VkResult result{vkCreateCommandPool(_device, create_info, nullptr, &command_pool)};

        assert_result_is_ok(result);

        return {_device, command_pool};
    }


    CommandBuffer Device::command_buffer(CommandBuffer::AllocateInfo const& allocate_info) const
    {
        assert(*this);

        VkCommandBuffer command_buffer;

        VkResult result{vkAllocateCommandBuffers(_device, allocate_info, &command_buffer)};

        assert_result_is_ok(result);

        return {_device, command_buffer};
    }


    Semaphore Device::semaphore(Semaphore::CreateInfo const& create_info) const
    {
        assert(*this);

        VkSemaphore semaphore;

        VkResult result{vkCreateSemaphore(_device, create_info, nullptr, &semaphore)};

        assert_result_is_ok(result);

        return {_device, semaphore};
    }


    Fence Device::fence(Fence::CreateInfo const& create_info) const
    {
        assert(*this);

        VkFence fence;

        VkResult result{vkCreateFence(_device, create_info, nullptr, &fence)};

        assert_result_is_ok(result);

        return {_device, fence};
    }

}  // namespace lue::vulkan

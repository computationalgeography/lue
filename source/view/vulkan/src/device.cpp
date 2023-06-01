#include "lue/vulkan/device.hpp"
#include "lue/vulkan/error.hpp"


namespace lue::vulkan {

    /*!
        @brief      Constructor for information about a single queue
        @sa         https://registry.khronos.org/vulkan/specs/1.3/html/chap5.html#VkDeviceQueueCreateInfo
    */
    Device::QueueCreateInfo::QueueCreateInfo(PhysicalDevice::QueueFamily const& queue_family):

        _create_info{},
        _queue_priorities{1}

    {
        _create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        _create_info.queueFamilyIndex = queue_family.idx();
        _create_info.queueCount = 1;
        _create_info.pQueuePriorities = _queue_priorities.data();
    }


    Device::QueueCreateInfo::operator VkDeviceQueueCreateInfo const*() const
    {
        return &_create_info;
    }


    Device::CreateInfo::CreateInfo(QueueCreateInfo&& queue_create_info):

        CreateInfo{std::move(queue_create_info), PhysicalDevice::Features{}}

    {
    }


    Device::CreateInfo::CreateInfo(
        QueueCreateInfo&& queue_create_info, PhysicalDevice::Features&& enabled_features):

        _enabled_features{std::move(enabled_features)},
        _create_info{}

    {
        _create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        _create_info.pQueueCreateInfos = queue_create_info;
        _create_info.queueCreateInfoCount = 1;
        _create_info.pEnabledFeatures = _enabled_features;
    }


    Device::CreateInfo::operator VkDeviceCreateInfo const*() const
    {
        return &_create_info;
    }


    Device::Queue::Queue(VkQueue queue):

        _queue{queue}

    {
    }


    Device::Queue::operator VkQueue() const
    {
        return _queue;
    }


    /*!
        @brief      .
        @param      .
        @return     .
        @sa         https://registry.khronos.org/vulkan/specs/1.3/html/chap5.html#vkCreateDevice
    */
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
    }


    Device::~Device()
    {
        ::vkDestroyDevice(_device, nullptr);
    }


    Device::Queue Device::queue(PhysicalDevice::QueueFamily const& queue_family)
    {
        VkQueue queue;

        ::vkGetDeviceQueue(_device, queue_family.idx(), 0, &queue);

        return queue;
    }

}  // namespace lue::vulkan

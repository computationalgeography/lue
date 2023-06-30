#include "lue/vulkan/device.hpp"
#include "lue/vulkan/error.hpp"
#include <algorithm>


namespace lue::vulkan {

    /*!
        @brief      Constructor for information about a single queue
        @sa         https://registry.khronos.org/vulkan/specs/1.3/html/chap5.html#VkDeviceQueueCreateInfo
    */
    Device::QueueCreateInfo::QueueCreateInfo(QueueFamily const& queue_family):

        _create_info{},
        _queue_priorities{1}

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


    Device::Queue::Queue(VkQueue queue):

        _queue{queue}

    {
    }


    /*!
        @warning    Do not use the returned pointer after this instance has gone out of scope
    */
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


    Device::Queue Device::queue(QueueFamily const& queue_family)
    {
        VkQueue queue;

        ::vkGetDeviceQueue(_device, queue_family, 0, &queue);

        return queue;
    }

    /*!
        @warning    Do not use the returned pointer after this instance has gone out of scope
    */
    Device::operator VkDevice() const
    {
        return _device;
    }

}  // namespace lue::vulkan

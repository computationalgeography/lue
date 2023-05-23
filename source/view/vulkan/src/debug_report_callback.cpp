#include "lue/vulkan/debug_report_callback.hpp"
#include "lue/vulkan/error.hpp"
#include <cassert>


namespace lue::vulkan {

    DebugReportCallback::CreateInfo::CreateInfo(
        VkDebugReportFlagsEXT const flags, PFN_vkDebugReportCallbackEXT callback):

        _create_info{}

    {
        _create_info.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
        _create_info.flags = flags;
        _create_info.pfnCallback = callback;
    }


    DebugReportCallback::CreateInfo::operator VkDebugReportCallbackCreateInfoEXT const*() const
    {
        return &_create_info;
    }


    PFN_vkCreateDebugReportCallbackEXT DebugReportCallback::_create_callback{nullptr};

    PFN_vkDestroyDebugReportCallbackEXT DebugReportCallback::_destroy_callback{nullptr};


    /*!
        @brief      .
        @param      .
        @return     .
        @exception  .

        It is required that the instance passed in stays alive longer than the DebugReportCallback
        instance created here.
    */
    DebugReportCallback::DebugReportCallback(Instance const& instance, CreateInfo&& create_info):

        _instance{instance},
        _callback{}

    {
        if (!_create_callback)
        {
            _create_callback = instance.function_pointer<PFN_vkCreateDebugReportCallbackEXT>(
                "vkCreateDebugReportCallbackEXT");
            assert(_create_callback);
        }

        if (!_destroy_callback)
        {
            _destroy_callback = instance.function_pointer<PFN_vkDestroyDebugReportCallbackEXT>(
                "vkDestroyDebugReportCallbackEXT");
            assert(_destroy_callback);
        }

        // Create the callback instance
        VkResult result{_create_callback(_instance, create_info, nullptr, &_callback)};
        assert_result_is_ok(result);
    }


    DebugReportCallback::~DebugReportCallback()
    {
        _destroy_callback(_instance, _callback, nullptr);
    }

}  // namespace lue::vulkan

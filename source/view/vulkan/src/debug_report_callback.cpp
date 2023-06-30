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


    /*!
        @warning    Do not use the returned pointer after this instance has gone out of scope
    */
    DebugReportCallback::CreateInfo::operator VkDebugReportCallbackCreateInfoEXT const*() const
    {
        return &_create_info;
    }


    PFN_vkCreateDebugReportCallbackEXT DebugReportCallback::_create_callback{nullptr};

    PFN_vkDestroyDebugReportCallbackEXT DebugReportCallback::_destroy_callback{nullptr};


    // DebugReportCallback::DebugReportCallback():

    //     _instance{VK_NULL_HANDLE},
    //     _callback{VK_NULL_HANDLE}

    // {
    //     assert(!_instance);
    //     assert(!_callback);
    //     assert(!*this);
    // }


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

        assert(*this);
    }


    DebugReportCallback::DebugReportCallback(DebugReportCallback&& other):

        _instance{std::move(other._instance)},
        _callback{std::move(other._callback)}

    {
        other._instance = VkInstance{};
        other._callback = VkDebugReportCallbackEXT{};

        assert(!other);
    }


    DebugReportCallback::~DebugReportCallback()
    {
        if (*this)
        {
            _destroy_callback(_instance, _callback, nullptr);
        }
    }


    DebugReportCallback& DebugReportCallback::operator=(DebugReportCallback&& other)
    {
        if (*this)
        {
            _destroy_callback(_instance, _callback, nullptr);
        }

        _instance = std::move(other._instance);
        _callback = std::move(other._callback);

        other._instance = VkInstance{};
        other._callback = VkDebugReportCallbackEXT{};

        assert(!other);

        return *this;
    }


    DebugReportCallback::operator bool() const
    {
        return _callback != VK_NULL_HANDLE;
    }

}  // namespace lue::vulkan

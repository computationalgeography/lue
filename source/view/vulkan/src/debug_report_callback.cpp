#include "lue/vulkan/debug_report_callback.hpp"
#include "lue/vulkan/error.hpp"
#include <cassert>


namespace lue::vulkan {

    PFN_vkCreateDebugReportCallbackEXT DebugReportCallback::_create_callback{nullptr};

    PFN_vkDestroyDebugReportCallbackEXT DebugReportCallback::_destroy_callback{nullptr};


    DebugReportCallback::DebugReportCallback():

        _instance{},
        _callback{}

    {
        assert(!_instance);
        assert(!is_valid());
    }


    /*!
        @brief      .
        @param      .
        @return     .
        @exception  .

        It is required that the instance passed in stays alive longer than the DebugReportCallback
        instance created here.
    */
    DebugReportCallback::DebugReportCallback(Instance& instance, CreateInfo&& create_info):

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

        assert(is_valid());
    }


    DebugReportCallback::DebugReportCallback(DebugReportCallback&& other):

        _instance{std::move(other._instance)},
        _callback{std::move(other._callback)}

    {
        other._instance = VkInstance{};
        other._callback = VkDebugReportCallbackEXT{};

        assert(!other.is_valid());
    }


    DebugReportCallback::~DebugReportCallback()
    {
        if (is_valid())
        {
            _destroy_callback(_instance, _callback, nullptr);

            _callback = VkDebugReportCallbackEXT{};
        }

        assert(!is_valid());
    }


    DebugReportCallback& DebugReportCallback::operator=(DebugReportCallback&& other)
    {
        if (is_valid())
        {
            _destroy_callback(_instance, _callback, nullptr);
        }

        _instance = std::move(other._instance);
        _callback = std::move(other._callback);

        other._instance = VkInstance{};
        other._callback = VkDebugReportCallbackEXT{};

        assert(!other.is_valid());

        return *this;
    }


    bool DebugReportCallback::is_valid() const
    {
        return _callback != VK_NULL_HANDLE;
    }

}  // namespace lue::vulkan

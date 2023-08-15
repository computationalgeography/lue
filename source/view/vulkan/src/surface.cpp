#include "lue/vulkan/surface.hpp"
#include <cassert>
#include <utility>


namespace lue::vulkan {

    Surface::Surface():

        _instance{},
        _surface{}

    {
        assert(!is_valid());
    }


    Surface::Surface(VkInstance instance, VkSurfaceKHR surface):

        _instance{instance},
        _surface{surface}

    {
        assert(is_valid());
    }


    Surface::Surface(Surface&& other):

        _instance{std::move(other._instance)},
        _surface{std::move(other._surface)}

    {
        other._instance = VkInstance{};
        other._surface = VkSurfaceKHR{};

        assert(!other.is_valid());
    }


    Surface::~Surface()
    {
        if (is_valid())
        {
            ::vkDestroySurfaceKHR(_instance, _surface, nullptr);
            _surface = VkSurfaceKHR{};
        }

        assert(!is_valid());
    }


    Surface& Surface::operator=(Surface&& other)
    {
        if (is_valid())
        {
            ::vkDestroySurfaceKHR(_instance, _surface, nullptr);
        }

        _instance = std::move(other._instance);
        _surface = std::move(other._surface);

        other._instance = VkInstance{};
        other._surface = VkSurfaceKHR{};

        assert(!other.is_valid());

        return *this;
    }


    bool Surface::is_valid() const
    {
        return _surface != VK_NULL_HANDLE;
    }


    /*!
        @warning    Do not use the returned pointer after this instance has gone out of scope
    */
    Surface::operator VkSurfaceKHR()
    {
        assert(is_valid());

        return _surface;
    }

}  // namespace lue::vulkan

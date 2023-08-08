#include "lue/vulkan/surface.hpp"
#include <cassert>
#include <utility>


namespace lue::vulkan {

    Surface::Surface():

        _instance{},
        _surface{}

    {
        assert(!*this);
    }


    Surface::Surface(VkInstance instance, VkSurfaceKHR surface):

        _instance{instance},
        _surface{surface}

    {
        assert(*this);
    }


    Surface::Surface(Surface&& other):

        _instance{std::move(other._instance)},
        _surface{std::move(other._surface)}

    {
        other._instance = VkInstance{};
        other._surface = VkSurfaceKHR{};

        assert(!other);
    }


    Surface::~Surface()
    {
        if (*this)
        {
            ::vkDestroySurfaceKHR(_instance, _surface, nullptr);
            _surface = VkSurfaceKHR{};
        }

        assert(!*this);
    }


    Surface& Surface::operator=(Surface&& other)
    {
        if (*this)
        {
            ::vkDestroySurfaceKHR(_instance, _surface, nullptr);
        }

        _instance = std::move(other._instance);
        _surface = std::move(other._surface);

        other._instance = VkInstance{};
        other._surface = VkSurfaceKHR{};

        assert(!other);

        return *this;
    }


    Surface::operator bool() const
    {
        return _surface != VK_NULL_HANDLE;
    }


    /*!
        @warning    Do not use the returned pointer after this instance has gone out of scope
    */
    Surface::operator VkSurfaceKHR() const
    {
        assert(*this);

        return _surface;
    }

}  // namespace lue::vulkan

#include "lue/vulkan/queue_family.hpp"
#include <cassert>
#include <utility>


namespace lue::vulkan {

    QueueFamily::Properties::Properties():

        _properties{}

    {
    }


    QueueFamily::Properties::Properties(VkQueueFamilyProperties&& properties):

        _properties{std::move(properties)}

    {
    }


    bool QueueFamily::Properties::graphics() const
    {
        return _properties.queueFlags & VK_QUEUE_GRAPHICS_BIT;
    }


    QueueFamily::QueueFamily():

        _idx{(std::uint32_t)-1}

    {
    }


    QueueFamily::QueueFamily(std::uint32_t const idx):

        _idx{idx}

    {
    }


    QueueFamily::operator std::uint32_t() const
    {
        return _idx;
    }


    void QueueFamilies::set_graphics_family(QueueFamily const& family)
    {
        assert(!_graphics_family.has_value());

        _graphics_family = family;
    }


    void QueueFamilies::set_present_family(QueueFamily const& family)
    {
        assert(!_present_family.has_value());

        _present_family = family;
    }


    QueueFamily const& QueueFamilies::graphics_family() const
    {
        assert(_graphics_family.has_value());

        return _graphics_family.value();
    }


    QueueFamily const& QueueFamilies::present_family() const
    {
        assert(_present_family.has_value());

        return _present_family.value();
    }


    bool QueueFamilies::is_complete() const
    {
        return _graphics_family.has_value() && _present_family.has_value();
    }

}  // namespace lue::vulkan

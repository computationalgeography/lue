#include "lue/vulkan/descriptor_set_layout.hpp"
#include <cassert>
#include <utility>


namespace lue::vulkan {

    DescriptorSetLayout::DescriptorSetLayout():

        _device{},
        _layout{}

    {
        assert(!is_valid());
    }


    DescriptorSetLayout::DescriptorSetLayout(VkDevice device, VkDescriptorSetLayout layout):

        _device{device},
        _layout{layout}

    {
        assert(is_valid());
    }


    DescriptorSetLayout::DescriptorSetLayout(DescriptorSetLayout&& other):

        _device{std::move(other._device)},
        _layout{std::move(other._layout)}

    {
        other._device = VkDevice{};
        other._layout = VkDescriptorSetLayout{};

        assert(!other.is_valid());
    }


    DescriptorSetLayout::~DescriptorSetLayout()
    {
        if (is_valid())
        {
            vkDestroyDescriptorSetLayout(_device, _layout, nullptr);

            _device = VkDevice{};
            _layout = VkDescriptorSetLayout{};
        }

        assert(!is_valid());
    }


    DescriptorSetLayout& DescriptorSetLayout::operator=(DescriptorSetLayout&& other)
    {
        if (is_valid())
        {
            vkDestroyDescriptorSetLayout(_device, _layout, nullptr);
        }

        _device = std::move(other._device);
        _layout = std::move(other._layout);

        other._device = VkDevice{};
        other._layout = VkDescriptorSetLayout{};

        assert(!other.is_valid());

        return *this;
    }


    bool DescriptorSetLayout::is_valid() const
    {
        return _layout != VK_NULL_HANDLE;
    }


    DescriptorSetLayout::operator VkDescriptorSetLayout()
    {
        assert(is_valid());

        return _layout;
    }


    DescriptorSetLayout::operator VkDescriptorSetLayout const*() const
    {
        assert(is_valid());

        return &_layout;
    }

}  // namespace lue::vulkan

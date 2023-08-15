#include "lue/vulkan/image_view.hpp"
#include <cassert>


namespace lue::vulkan {

    ImageView::ImageView():

        _device{},
        _image_view{}

    {
        assert(!is_valid());
    }


    ImageView::ImageView(VkDevice device, VkImageView image_view):

        _device{device},
        _image_view{image_view}

    {
        assert(is_valid());
    }


    ImageView::ImageView(ImageView&& other):

        _device{std::move(other._device)},
        _image_view{std::move(other._image_view)}

    {
        other._device = VkDevice{};
        other._image_view = VkImageView{};

        assert(!other.is_valid());
    }


    ImageView::~ImageView()
    {
        if (is_valid())
        {
            vkDestroyImageView(_device, _image_view, nullptr);
            _device = VkDevice{};
            _image_view = VkImageView{};
        }

        assert(!is_valid());
    }


    ImageView& ImageView::operator=(ImageView&& other)
    {
        if (is_valid())
        {
            vkDestroyImageView(_device, _image_view, nullptr);
        }

        _device = std::move(other._device);
        _image_view = std::move(other._image_view);

        other._device = VkDevice{};
        other._image_view = VkImageView{};

        assert(!other.is_valid());

        return *this;
    }


    bool ImageView::is_valid() const
    {
        return _image_view != VK_NULL_HANDLE;
    }


    ImageView::operator VkImageView()
    {
        assert(is_valid());

        return _image_view;
    }

}  // namespace lue::vulkan

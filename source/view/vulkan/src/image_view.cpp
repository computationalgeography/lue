#include "lue/vulkan/image_view.hpp"
#include <cassert>


namespace lue::vulkan {

    ImageView::CreateInfo::CreateInfo():

        _create_info{}

    {
        _create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    }


    /*!
        @warning    Do not use the returned pointer after this instance has gone out of scope
    */
    ImageView::CreateInfo::operator VkImageViewCreateInfo const*() const
    {
        return &_create_info;
    }


    VkImageViewCreateInfo& ImageView::CreateInfo::operator*()
    {
        return _create_info;
    }


    ImageView::ImageView():

        _device{},
        _image_view{}

    {
        assert(!*this);
    }


    ImageView::ImageView(VkDevice device, VkImageView image_view):

        _device{device},
        _image_view{image_view}

    {
        assert(*this);
    }


    ImageView::ImageView(ImageView&& other):

        _device{std::move(other._device)},
        _image_view{std::move(other._image_view)}

    {
        other._device = VkDevice{};
        other._image_view = VkImageView{};

        assert(!other);
    }


    ImageView::~ImageView()
    {
        if (*this)
        {
            vkDestroyImageView(_device, _image_view, nullptr);
            _device = VkDevice{};
            _image_view = VkImageView{};
        }

        assert(!*this);
    }


    ImageView& ImageView::operator=(ImageView&& other)
    {
        if (*this)
        {
            vkDestroyImageView(_device, _image_view, nullptr);
        }

        _device = std::move(other._device);
        _image_view = std::move(other._image_view);

        other._device = VkDevice{};
        other._image_view = VkImageView{};

        assert(!other);

        return *this;
    }


    ImageView::operator bool() const
    {
        return _image_view != VK_NULL_HANDLE;
    }


    ImageView::operator VkImageView() const
    {
        assert(*this);

        return _image_view;
    }

}  // namespace lue::vulkan

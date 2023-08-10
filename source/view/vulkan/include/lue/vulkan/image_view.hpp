#pragma once
#include "lue/vulkan/structure.hpp"
#include <vulkan/vulkan.h>
#include <vector>


namespace lue::vulkan {

    class ImageView
    {

        public:

            using CreateInfo = Structure<VkImageViewCreateInfo, VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO>;


            ImageView();

            ImageView(VkDevice device, VkImageView image_view);

            ImageView(ImageView const&) = delete;

            ImageView(ImageView&& other);

            ~ImageView();

            ImageView& operator=(ImageView const&) = delete;

            ImageView& operator=(ImageView&& other);

            operator bool() const;

            operator VkImageView() const;

        private:

            static_assert(std::is_pointer_v<VkDevice>);

            VkDevice _device;

            static_assert(std::is_pointer_v<VkImageView>);

            VkImageView _image_view;
    };


    using ImageViews = std::vector<ImageView>;

}  // namespace lue::vulkan

#pragma once
#include <vulkan/vulkan.h>
#include <vector>


namespace lue::vulkan {

    class ImageView
    {

        public:

            class CreateInfo
            {

                public:

                    CreateInfo();

                    CreateInfo(CreateInfo const&) = delete;

                    CreateInfo(CreateInfo&&) = default;

                    ~CreateInfo() = default;

                    CreateInfo& operator=(CreateInfo const&) = delete;

                    CreateInfo& operator=(CreateInfo&&) = default;

                    operator VkImageViewCreateInfo const*() const;

                    VkImageViewCreateInfo& operator*();

                private:

                    static_assert(!std::is_pointer_v<VkImageViewCreateInfo>);

                    VkImageViewCreateInfo _create_info;
            };


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

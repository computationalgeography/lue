#pragma once
#include <vulkan/vulkan.h>
#include <type_traits>


namespace lue::vulkan {

    class PipelineLayout
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

                    operator VkPipelineLayoutCreateInfo const*() const;

                private:

                    VkPipelineLayoutCreateInfo _create_info;
            };


            PipelineLayout();

            PipelineLayout(VkDevice device, VkPipelineLayout pipeline_layout);

            PipelineLayout(PipelineLayout const&) = delete;

            PipelineLayout(PipelineLayout&& other);

            ~PipelineLayout();

            PipelineLayout& operator=(PipelineLayout const&) = delete;

            PipelineLayout& operator=(PipelineLayout&& other);

            operator bool() const;

            operator VkPipelineLayout() const;

        private:

            static_assert(std::is_pointer_v<VkDevice>);

            VkDevice _device;

            static_assert(std::is_pointer_v<VkPipelineLayout>);

            VkPipelineLayout _pipeline_layout;
    };

}  // namespace lue::vulkan

#pragma once
#include "lue/vulkan/instance_create_info.hpp"


namespace lue::vulkan {

    class Instance
    {

        public:

            Instance(InstanceCreateInfo&& instance_create_info);

            Instance(Instance const&) = delete;

            Instance(Instance&&) = default;

            ~Instance();

            Instance& operator=(Instance const&) = delete;

            Instance& operator=(Instance&&) = default;

        private:

            InstanceCreateInfo _instance_create_info;

            VkInstance _instance;
    };

}  // namespace lue::vulkan

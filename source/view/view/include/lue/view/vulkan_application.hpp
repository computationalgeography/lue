#pragma once
#include "lue/view/application.hpp"


namespace lue::view {

    class VulkanApplication: public Application
    {

        public:

            VulkanApplication(std::vector<std::string> const& arguments);

            ~VulkanApplication();

        protected:

            int run_implementation() final;
    };

}  // namespace lue::view

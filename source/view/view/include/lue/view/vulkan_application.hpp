#pragma once
#include "lue/view/application.hpp"


namespace lue::view {

    class VulkanApplication: public Application
    {

        public:

            VulkanApplication(int argc, char const* const* argv);

            ~VulkanApplication();

        protected:

            auto run_implementation() -> int final;
    };

}  // namespace lue::view

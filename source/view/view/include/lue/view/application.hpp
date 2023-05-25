#pragma once
#include "lue/view/show_main_menu_bar.hpp"
#include "lue/utility/application.hpp"


namespace lue::view {

    class Application: public utility::Application

    {

        public:

        protected:

            Application(std::vector<std::string> const& arguments);
    };

}  // namespace lue::view

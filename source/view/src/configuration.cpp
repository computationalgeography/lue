#include "lue/view/configuration.hpp"


namespace lue {
    namespace view {

        Configuration::Configuration():

            _show_details{false}

        {
        }


        bool Configuration::show_details() const
        {
            return _show_details;
        }


        bool& Configuration::show_details()
        {
            return _show_details;
        }

    }  // namespace view
}  // namespace lue

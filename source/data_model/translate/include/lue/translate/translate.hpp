#pragma once
#include "lue/gdal/error.hpp"
#include "lue/utility/application.hpp"


namespace lue::utility {

    class Translate: public Application

    {

        public:

            Translate(int argc, char const* const* argv);

        private:

            static auto export_data(std::vector<std::string> const& arguments) -> CommandPtr;

            gdal::QuietErrorHandler _gdal_error_handler;
    };

}  // namespace lue::utility

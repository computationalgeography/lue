#pragma once
#include "lue/gdal/error.hpp"
#include "lue/utility/application.hpp"


namespace lue::utility {

    class Translate: public Application

    {

        public:

            explicit Translate(std::vector<std::string> const& arguments);

        private:

            // static CommandPtr import_data      (std::vector<std::string> const&
            //                                         arguments);

            static auto export_data(std::vector<std::string> const& arguments) -> CommandPtr;

            // void           print_format        (std::string const& dataset_name,
            //                                     std::string const& format);

            gdal::QuietErrorHandler _gdal_error_handler;
    };

}  // namespace lue::utility

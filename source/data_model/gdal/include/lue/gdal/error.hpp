#pragma once


namespace lue::gdal {


    class QuietErrorHandler
    {

        public:

            QuietErrorHandler();

            QuietErrorHandler(QuietErrorHandler const&) = delete;

            QuietErrorHandler(QuietErrorHandler&&) = delete;

            ~QuietErrorHandler();

            auto operator=(QuietErrorHandler const&) -> QuietErrorHandler = delete;

            auto operator=(QuietErrorHandler&&) -> QuietErrorHandler = delete;
    };

}  // namespace lue::gdal

#pragma once


namespace lue::gdal {


    class QuietErrorHandler
    {

        public:

            QuietErrorHandler();

            QuietErrorHandler(QuietErrorHandler const& other) = delete;

            ~QuietErrorHandler();

            auto operator=(QuietErrorHandler const& other) -> QuietErrorHandler = delete;
    };

}  // namespace lue::gdal

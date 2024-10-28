#pragma once


namespace lue::gdal {


    /*!
        @brief      Class which installs (pushes) a quiet error handler upon construction and de-installs
                    (pops) it again upon destruction
    */
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

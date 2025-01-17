#include "lue/gdal/error.hpp"
#include <gdal_priv.h>


namespace lue::gdal {

    QuietErrorHandler::QuietErrorHandler()
    {
        CPLPushErrorHandler(CPLQuietErrorHandler);
    }


    QuietErrorHandler::~QuietErrorHandler()
    {
        CPLPopErrorHandler();
    }

}  // namespace lue::gdal

#pragma once
#include "lue/hl/raster_domain.hpp"
#include "lue/hl/time_series_domain.hpp"


namespace lue {
namespace hl {

class RasterStackDomain
{

public:

                   RasterStackDomain   ()=default;

                   RasterStackDomain   (RasterStackDomain const&)=default;

                   RasterStackDomain   (RasterStackDomain&&)=default;

                   RasterStackDomain   (
                                    TimeSeriesDomain const& time_series_domain,
                                    RasterDomain const& raster_domain);

                   ~RasterStackDomain  ()=default;

    RasterStackDomain& operator=       (RasterStackDomain const&)=default;

    RasterStackDomain& operator=       (RasterStackDomain&&)=default;

    TimeSeriesDomain const& time_series_domain() const;

    RasterDomain const& raster_domain  () const;

private:

    TimeSeriesDomain _time_series_domain;

    RasterDomain   _raster_domain;

};


bool               operator==          (RasterStackDomain const& lhs,
                                        RasterStackDomain const& rhs);

bool               operator<           (RasterStackDomain const& lhs,
                                        RasterStackDomain const& rhs);

}  // namespace hl
}  // namespace lue

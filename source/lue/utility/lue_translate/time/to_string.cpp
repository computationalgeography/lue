#include "lue/utility/lue_translate/time/to_string.h"
#include "boost/format.hpp"


namespace lue {

std::string time_point_to_iso_8601_string(
    MonthTimePoint const& time_point)
{
    return boost::str(boost::format("%04d-%02d")
        % time_point.year()
        % time_point.month());
}


std::string time_duration_to_iso_8601_string(
    MonthDuration const& time_duration)
{
    return boost::str(boost::format("P%dM")
        % time_duration.count());
}

}  // namespace lue

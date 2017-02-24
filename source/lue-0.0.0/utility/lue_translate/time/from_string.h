#pragma once
#include <string>
#include <tuple>
#include "lue/utility/lue_translate/time/month_duration.h"
#include "lue/utility/lue_translate/time/month_time_point.h"


namespace lue {

std::tuple<bool, MonthTimePoint, MonthDuration>
                   parse_months        (std::string const& time_point_string,
                                        std::string const& duration_string);

}  // namespace lue

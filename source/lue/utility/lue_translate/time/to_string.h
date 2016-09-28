#pragma once
#include <string>
#include "lue/utility/lue_translate/time/month_duration.h"
#include "lue/utility/lue_translate/time/month_time_point.h"


namespace lue {

std::string        time_point_to_iso_8601_string(
                                        MonthTimePoint const& time_point);

std::string        time_duration_to_iso_8601_string(
                                        MonthDuration const& time_duration);

}  // namespace lue

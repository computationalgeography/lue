#pragma once


#include "lue/framework/algorithm/policy/default_policies.hpp"
#include "lue/framework/algorithm/policy/default_value_policies.hpp"
#include "lue/framework/algorithm/policy/policies.hpp"


// Domain policies
#include "lue/framework/algorithm/policy/all_values_within_domain.hpp"


// Input policies
#include "lue/framework/algorithm/policy/detect_no_data_by_nan.hpp"
#include "lue/framework/algorithm/policy/detect_no_data_by_value.hpp"
#include "lue/framework/algorithm/policy/input_policies.hpp"
#include "lue/framework/algorithm/policy/skip_no_data.hpp"


// Output policies
#include "lue/framework/algorithm/policy/dont_mark_no_data.hpp"
#include "lue/framework/algorithm/policy/mark_no_data_by_nan.hpp"
#include "lue/framework/algorithm/policy/mark_no_data_by_value.hpp"
#include "lue/framework/algorithm/policy/output_policies.hpp"


// Halo policies
#include "lue/framework/algorithm/policy/fill_halo_with_constant_value.hpp"
#include "lue/framework/algorithm/policy/flow_direction_halo.hpp"


#include "lue/framework/algorithm/policy/keep_no_data_focus_element.hpp"
#include "lue/framework/algorithm/policy/skip_out_of_image.hpp"


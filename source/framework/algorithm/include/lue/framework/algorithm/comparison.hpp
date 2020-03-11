#pragma once
#include "lue/framework/core/component/partitioned_array.hpp"
#include "lue/framework/algorithm/close_to.hpp"
#include "lue/framework/algorithm/equal_to.hpp"
#include "lue/framework/algorithm/greater_than.hpp"
#include "lue/framework/algorithm/greater_than_equal_to.hpp"
#include "lue/framework/algorithm/less_than.hpp"
#include "lue/framework/algorithm/less_than_equal_to.hpp"
#include "lue/framework/algorithm/not_equal_to.hpp"
#include "lue/framework/algorithm/operator.hpp"


namespace lue {

LUE_BINARY_COMPARISON_OPERATOR(==, equal_to)
LUE_BINARY_COMPARISON_OPERATOR(>, greater_than)
LUE_BINARY_COMPARISON_OPERATOR(>=, greater_than_equal_to)
LUE_BINARY_COMPARISON_OPERATOR(<, less_than)
LUE_BINARY_COMPARISON_OPERATOR(<=, less_than_equal_to)
LUE_BINARY_COMPARISON_OPERATOR(!=, not_equal_to)

}  // namespace lue

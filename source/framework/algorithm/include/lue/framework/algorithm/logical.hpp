#pragma once
#include "lue/framework/algorithm/logical_and.hpp"
#include "lue/framework/algorithm/logical_inclusive_or.hpp"
#include "lue/framework/algorithm/logical_not.hpp"
#include "lue/framework/algorithm/operator.hpp"


namespace lue {

LUE_BINARY_LOGICAL_OPERATOR(&&, logical_and)
LUE_BINARY_LOGICAL_OPERATOR(||, logical_inclusive_or)
LUE_UNARY_LOGICAL_OPERATOR(!, logical_not)

}  // namespace lue

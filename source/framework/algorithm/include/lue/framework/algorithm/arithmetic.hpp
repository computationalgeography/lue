#pragma once
#include "lue/framework/core/component/partitioned_array.hpp"
#include "lue/framework/algorithm/add.hpp"
#include "lue/framework/algorithm/divide.hpp"
#include "lue/framework/algorithm/multiply.hpp"
#include "lue/framework/algorithm/operator.hpp"
#include "lue/framework/algorithm/subtract.hpp"


namespace lue {

LUE_BINARY_ARITHMETIC_OPERATOR(+, add)
LUE_BINARY_ARITHMETIC_OPERATOR(/, divide)
LUE_BINARY_ARITHMETIC_OPERATOR(*, multiply)
LUE_BINARY_ARITHMETIC_OPERATOR(-, subtract)

}  // namespace lue

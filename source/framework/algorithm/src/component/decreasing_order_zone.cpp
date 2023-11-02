#include "lue/framework/algorithm/component/decreasing_order_zone.hpp"
// #include <hpx/serialization/std_tuple.hpp>


#define REGISTER(Zone, Value, rank)                                                                          \
                                                                                                             \
    LUE_REGISTER_DECREASING_ORDER_ZONE_ACTION_DECLARATIONS(Zone, Value, rank)                                \
    LUE_REGISTER_DECREASING_ORDER_ZONE_ACTIONS(Zone, Value, rank)

// std::int32_t std::int64_t
// std::uint8_t std::uint32_t std::uint64_t

REGISTER(/* std:: */ uint8_t, /* std:: */ uint32_t, 2)

#undef REGISTER

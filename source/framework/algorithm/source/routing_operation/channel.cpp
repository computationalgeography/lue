#include "lue/framework/core/define.hpp"
#include <hpx/channel.hpp>
#include <hpx/include/components.hpp>
#include <vector>


using lue_CellsIdxs = std::vector<lue::Index>;

HPX_REGISTER_CHANNEL(lue_CellsIdxs);

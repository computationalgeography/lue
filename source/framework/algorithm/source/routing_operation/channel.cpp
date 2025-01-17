// #include "lue/framework/core/define.hpp"
// #include <hpx/channel.hpp>
// #include <vector>

// TODO Which HPX register to use for HPX_REGISTER_CHANNEL. We should not have to include all this:
#include "lue/framework/algorithm/definition/flow_accumulation3.hpp"


using lue_CellsIdxs = std::vector<lue::Index>;

HPX_REGISTER_CHANNEL(lue_CellsIdxs);

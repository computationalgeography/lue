#include "lue/framework/io/io_strategy.hpp"


namespace lue {

    IOStrategy::IOStrategy(PartitionGrouping grouping):

        _grouping{grouping}

    {
    }


    auto IOStrategy::grouping() const -> PartitionGrouping
    {
        return _grouping;
    }

}  // namespace lue

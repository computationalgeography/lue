#pragma once
#include "lue/framework/core/type_traits.hpp"
#include "lue/framework/algorithm/component/array_partition_io.hpp"
#include "lue/framework/core/array.hpp"


namespace lue::detail {

    template<
        typename Index,
        Rank r,
        typename V>
    class ArrayTraits<lue::Array<lue::ArrayPartitionIO<Index, r, V>, r>>
    {

        public:

            using Element = V;

            constexpr static Rank rank = r;

            using Shape = typename lue::Array<lue::ArrayPartitionIO<Index, r, V>, r>::Shape;

            template<
                typename V_,
                Rank r_>
            using Component = lue::ArrayPartitionIO<Index, r_, V_>;

    };

}  // namespacel lue::detail

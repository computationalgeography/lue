#pragma once
#include "lue/framework/algorithm/detail/communicator.hpp"
#include "lue/framework/partitioned_array.hpp"
#include "lue/framework/core/array.hpp"


namespace lue::detail {

    template<
        typename Communicator,
        Rank rank>
    class CommunicatorArray:
        public Array<Communicator, rank>
    {

        public:

            static_assert(rank == 2);

            using Base = Array<Communicator, rank>;
            using Shape = typename Base::Shape;


            CommunicatorArray(
                std::string const& scope,
                Localities<rank> const& localities)

                : Base{localities.shape()}

            {
                auto [extent0, extent1] = localities.shape();

                for(Index idx0 = 0; idx0 < extent0; ++idx0) {
                    for(Index idx1 = 0; idx1 < extent1; ++idx1)
                    {
                        (*this)(idx0, idx1) =
                            Communicator{localities(idx0, idx1), scope, localities.shape(), {idx0, idx1}};
                    }
                }
            }


        private:

    };

}  // namespace lue::detail

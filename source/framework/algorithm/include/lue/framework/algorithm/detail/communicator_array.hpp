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
                std::string const& basename,
                Localities<rank> const& localities):

                Base{localities.shape()},
                _basename{basename + std::to_string(invocation_id++)}

            {
                auto [extent0, extent1] = localities.shape();

                for(Index idx0 = 0; idx0 < extent0; ++idx0) {
                    for(Index idx1 = 0; idx1 < extent1; ++idx1)
                    {
                        (*this)(idx0, idx1) =
                            Communicator{localities(idx0, idx1), _basename, localities.shape(), {idx0, idx1}};
                    }
                }
            }


            // ~CommunicatorArray()
            // {
            //     // Free up AGAS resources
            //     unregister().wait();
            // }


            hpx::future<void> unregister()
            {
                auto [extent0, extent1] = this->shape();

                std::vector<hpx::future<void>> fs{};
                fs.reserve(nr_elements(this->shape()));

                for(Index idx0 = 0; idx0 < extent0; ++idx0) {
                    for(Index idx1 = 0; idx1 < extent1; ++idx1)
                    {
                        fs.push_back((*this)(idx0, idx1).unregister(_basename));
                    }
                }

                return hpx::when_all(fs).then([]([[maybe_unused]] auto&& fs) { return; });
            }


        private:

            inline static std::size_t invocation_id = 0;

            std::string _basename;

    };

}  // namespace lue::detail

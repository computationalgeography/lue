#pragma once
#include "lue/framework/algorithm/detail/communicator.hpp"
#include "lue/framework/core/array.hpp"
#include "lue/framework/partitioned_array_decl.hpp"  // Localities


namespace lue::detail {

    template<typename Communicator, Rank rank>
    class CommunicatorArray: public Array<Communicator, rank>
    {

            static_assert(rank == 2);

        private:

            using Base = Array<Communicator, rank>;

        public:

            using Shape = typename Base::Shape;


            /*!
                @brief      Construct an instance, creating a communicator per cell
                @param      .
            */
            CommunicatorArray(std::string const& basename, Localities<rank> const& localities):

                Base{localities.shape()},
                _basename{basename + std::to_string(invocation_id++)}

            {
                auto [extent0, extent1] = localities.shape();

                for (Index idx0 = 0; idx0 < extent0; ++idx0)
                {
                    for (Index idx1 = 0; idx1 < extent1; ++idx1)
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


            auto unregister() -> hpx::future<void>
            {
                auto [extent0, extent1] = this->shape();

                std::vector<hpx::future<void>> futures{};
                futures.reserve(nr_elements(this->shape()));

                for (Index idx0 = 0; idx0 < extent0; ++idx0)
                {
                    for (Index idx1 = 0; idx1 < extent1; ++idx1)
                    {
                        futures.push_back((*this)(idx0, idx1).unregister(_basename));
                    }
                }

                return hpx::when_all(futures).then([]([[maybe_unused]] auto&& futures) { return; });
            }


        private:

            inline static std::size_t invocation_id = 0;

            std::string _basename;
    };

}  // namespace lue::detail

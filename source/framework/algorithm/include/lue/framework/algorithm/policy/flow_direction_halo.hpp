#pragma once
#include "lue/framework/algorithm/flow_direction.hpp"
#include "lue/framework/core/define.hpp"


namespace lue::policy {

    template<
        typename FlowDirectionElement>
    class FlowDirectionHalo
    {

        public:

            static constexpr FlowDirectionElement north_west_corner() noexcept
            {
                return north_west<FlowDirectionElement>;
            }


            static constexpr FlowDirectionElement north_east_corner() noexcept
            {
                return north_east<FlowDirectionElement>;
            }


            static constexpr FlowDirectionElement south_west_corner() noexcept
            {
                return south_west<FlowDirectionElement>;
            }


            static constexpr FlowDirectionElement south_east_corner() noexcept
            {
                return south_east<FlowDirectionElement>;
            }


            static constexpr FlowDirectionElement west_side() noexcept
            {
                return west<FlowDirectionElement>;
            }


            static constexpr FlowDirectionElement east_side() noexcept
            {
                return east<FlowDirectionElement>;
            }


            static constexpr FlowDirectionElement north_side() noexcept
            {
                return north<FlowDirectionElement>;
            }


            static constexpr FlowDirectionElement south_side() noexcept
            {
                return south<FlowDirectionElement>;
            }

        private:

    };


    namespace detail {

        template<
            typename E>
        class TypeTraits<
            FlowDirectionHalo<E>>
        {

            public:

                using Element = E;

                template<
                    typename E_>
                using Policy = FlowDirectionHalo<E_>;

        };

    }  // namespace detail
}  // namespace lue::policy

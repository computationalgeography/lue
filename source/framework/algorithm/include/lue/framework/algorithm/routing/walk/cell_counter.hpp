#pragma once
#include "lue/framework/algorithm/routing_operation_export.hpp"
#include "lue/framework/core/define.hpp"
#include <hpx/serialization.hpp>


namespace lue {

    class LUE_ROUTING_OPERATION_EXPORT CellCounter
    {

        public:

            CellCounter();

            CellCounter(Count max_nr_cells_to_visit);

            auto operator++() -> Count&;

            [[nodiscard]] auto keep_walking() const -> bool;

        protected:

            friend class hpx::serialization::access;

            template<typename Archive>
            void serialize(Archive& archive, [[maybe_unused]] unsigned int const version)
            {
                // clang-format off
                archive & _nr_cells_visited & _max_nr_cells_to_visit;
                // clang-format on
            }

        private:

            Count _nr_cells_visited;

            Count _max_nr_cells_to_visit;
    };

}  // namespace lue

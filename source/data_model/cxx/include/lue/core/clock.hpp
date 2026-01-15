#pragma once
#include "lue/core/time.hpp"


namespace lue::data_model {

    class LUE_DATA_MODEL_EXPORT Clock
    {

        public:

            Clock(time::Unit unit, time::TickPeriodCount nr_units);

            Clock(time::Epoch epoch, time::Unit unit, time::TickPeriodCount nr_units);

            Clock(Clock const& other) = default;

            Clock(Clock&& other) = default;

            ~Clock() = default;

            auto operator=(Clock const& other) -> Clock& = default;

            auto operator=(Clock&& other) -> Clock& = default;

            auto operator==(Clock const& other) const -> bool;

            auto operator!=(Clock const& other) const -> bool;

            auto operator<(Clock const& other) const -> bool;

            auto epoch() const -> time::Epoch const&;

            auto unit() const -> time::Unit;

            auto nr_units() const -> time::TickPeriodCount;

        private:

            time::Epoch _epoch;

            time::Unit _unit;

            time::TickPeriodCount _nr_units;
    };

}  // namespace lue::data_model

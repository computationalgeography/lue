#pragma once
#include "lue/info/time/time_box.hpp"


namespace lue::data_model {

    /*!
        @brief      Class for representing time box domains that are discretized using regular grids

        Time cells are time boxes with additional information about the number of cells in which each box is
        discretized.
    */
    class LUE_DATA_MODEL_EXPORT TimeCell: public TimeBox
    {

        public:

            using Count = same_shape::constant_shape::Value;

            explicit TimeCell(hdf5::Group const& parent);

            TimeCell(TimeBox&& time_box, Count count);

            TimeCell(TimeCell const& other) = default;

            TimeCell(TimeCell&& other) = default;

            ~TimeCell() override = default;

            auto operator=(TimeCell const& other) -> TimeCell& = default;

            auto operator=(TimeCell&& other) -> TimeCell& = default;

            auto nr_counts() const -> data_model::Count;

            auto count() const -> Count const&;

            auto count() -> Count&;

        private:

            //! For each time box a count
            Count _count;
    };


    auto create_time_cell(hdf5::Group& parent) -> TimeCell;


    template<>
    class ValueTraits<TimeCell>
    {

        public:

            using Element = typename ValueTraits<LocationInTime>::Element;
    };

}  // namespace lue::data_model

#pragma once
#include "lue/data_model/export.hpp"
#include <optional>
#include <string>


namespace lue {
    namespace data_model {
        namespace time {

            enum class Calendar {

                gregorian,

            };


            /*!
                @brief      Epoch defining the start point of an era

                All locations in time are anchored to an epoch. Only given an epoch,
                which is part of a clock, can locations in time be mapped to 'real'
                time (for some definition of real).

                There are many kinds of epochs.

                LUE extents the definition of epoch a bit by introducing an additional
                origin. Although an epoch kind defines the start of an era, with
                origin the user can refine this by marking a certain location in
                time within the era as the real epoch of the time points. For example,
                the Common Era epoch defines the year one as the starting point
                of its era. This class supports using, for example, the first day
                of this year as the epoch, or 'now'.
            */
            class LUE_DATA_MODEL_EXPORT Epoch
            {

                public:

                    enum class Kind {

                        common_era,

                        formation_of_earth,

                    };


                    Epoch();

                    explicit Epoch(Kind kind);

                    Epoch(Kind kind, std::string const& origin);

                    Epoch(Kind kind, std::string const& origin, Calendar calendar);

                    Epoch(Epoch const&) = default;

                    Epoch(Epoch&&) = default;

                    ~Epoch() = default;

                    Epoch& operator=(Epoch const&) = default;

                    Epoch& operator=(Epoch&&) = default;

                    Kind kind() const;

                    std::optional<std::string> const& origin() const;

                    std::optional<Calendar> const& calendar() const;

                private:

                    //! The epoch kind, representing some origin of an era in time
                    Kind _kind;

                    //! The real epoch, at some distance in time from the epoch kind's origin
                    std::optional<std::string> _origin;

                    //! An optional calendar, to be able to interpret the origin
                    std::optional<Calendar> _calendar;
            };

        }  // namespace time
    }  // namespace data_model
}  // namespace lue

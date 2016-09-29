#pragma once
#include <cstdlib>


namespace lue {

class MonthDuration
{

public:

    using Count = unsigned int;

                   MonthDuration       ();

                   MonthDuration       (Count const count);

                   MonthDuration       (MonthDuration const& other)=default;

                   MonthDuration       (MonthDuration&& other)=default;

                   ~MonthDuration      ()=default;

    MonthDuration& operator=           (MonthDuration const& lhs)=default;

    MonthDuration& operator=           (MonthDuration&& lhs)=default;

    MonthDuration& operator*=          (size_t const multiplier);

    Count          count               () const;

private:

    // Number of months in the duration. Can be negative.
    Count          _count;

};


MonthDuration      operator*           (MonthDuration const& duration,
                                        size_t const multiplier);

MonthDuration      operator*           (size_t const multiplier,
                                        MonthDuration const& duration);

}  // namespace lue

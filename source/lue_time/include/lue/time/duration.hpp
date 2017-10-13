#pragma once


namespace lue {
namespace time {

/*!
    @brief      Class for representing an amount of time
    @tparam     TickPeriod Type for representing the length of a Clock
                tick period

    A Duration instance is represented by a number of TickPeriod units.
*/
template<
    typename TickPeriod>
class Duration
{

public:

    using Count = std::size_t;

                   Duration            (Count const nr_ticks);

                   Duration            (Duration const&)=default;

                   Duration            (Duration&&)=default;

                   ~Duration           ()=default;

    Duration&      operator=           (Duration const&)=default;

    Duration&      operator=           (Duration&&)=default;

    Count          nr_ticks            () const;

private:

    //! Number of tick period ticks in duration
    Count          _nr_ticks;

};


template<
    typename TickPeriod>
inline Duration<TickPeriod>::Duration(
    Count const nr_ticks)

    : _nr_ticks{nr_ticks}

{
}


template<
    typename TickPeriod>
inline typename Duration<TickPeriod>::Count
    Duration<TickPeriod>::nr_ticks() const
{
    return _nr_ticks;
}


template<
    typename TickPeriod>
inline bool operator==(
        Duration<TickPeriod> const& lhs,
        Duration<TickPeriod> const& rhs)
{
    return lhs.nr_ticks() == rhs.nr_ticks();
}

}  // namespace time
}  // namespace lue

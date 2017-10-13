[TOC]

# lue_time  {#lue_time}
Time is a one-dimensional domain. Different applications have different requirements for storing time. In some cases time is represented by light years, while in other cases it is represented by nanoseconds. The LUE time library contains various classes for handling time.


Notes:
```cpp
// Each nr_units a tick. This determines the resolution of the clock, e.g.:
// - every 10 seconds
// - every 1 light years
TickPeriod<Unit>::TickPeriod(
    std::size_t const nr_units)


// Duration is defined in terms of `counts of tick-period units`, like:
// - 5 * (every 10 seconds)
// - 500 * (every 1 light years)
Duration<TickPeriod<Unit>>::Duration(
    std::size_t const nr_tick_period_units)


// The Clock defines a open period of time since an epoch, and into
// the future. Not sure how to define Epoch. Leave it undefined for now.
// Mapping the clock to some calendar may solve this.
Clock<TickPeriod>::Clock()


// A TimePoint is defined by a Clock and a Duration since the epoch of
// the clock.
// - 100 * Clock::tick_period, where tick_period can be 10 seconds, or 1
//   light years, or ...
TimePoint<Clock> TimePoint(
    Duration const& duration)


// A Calendar maps a clock to real-world time. There are differend kinds:
// - Gregorian calendar
// - Julian calendar
// - Islamic calendar
// - ...


// Pass an epoch and a clock to a calendar. Then the calendar instance
// can translate time points to calendar dates.
GregorianCalendar<Clock> GregorianCalendar(
    Epoch const& epoch,
    Clock const& clock)
calendar.date(time_point);

auto date = parse<Calendar>(calendar, string)



// See also:
// - Geological Calendar: map geological lifetime of the earth to a
//   single year
// - Cosmic Calendar: map universe clock to a single year

// Handle:
// - periodization



// Store in dataset:
// - Clock info
//     - unit
//     - tick_period
// - Calendar info
//     - kind
//     - epoch






```

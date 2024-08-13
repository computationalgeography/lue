[TOC]

# Time  {#lue_time}

Time is a one-dimensional domain. Different applications have different
requirements for storing time. In some cases time is represented by
light years, while in other cases it is represented by nanoseconds. The
LUE time library contains various classes for handling time.

Notes:

```cpp
// A Calendar maps a clock to real-world time. There are different kinds:
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

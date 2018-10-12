#pragma once
#include <array>
#include <cassert>
#include <iostream>


namespace lue {
namespace benchmark {

/*!
    @brief      Return a string representation of a time point

    The string is formatted according to ISO 8601: yyyy-mm-ddThh:mm:ss+zzzz.
    Example: 2018-10-11T15:55:59+0200
*/
std::string to_iso_string(
    TimeInterval::TimePoint const& time_point)
{
    static std::size_t const max_nr_characters = 30;
    static std::array<char, max_nr_characters> buffer;

    auto const c_time_point = std::chrono::system_clock::to_time_t(time_point);
    auto const time = std::localtime(&c_time_point);

    auto nr_bytes_written =
        std::strftime(buffer.data(), max_nr_characters, "%FT%T%z", time);
    assert(nr_bytes_written <= max_nr_characters);

    return std::string(buffer.begin(), buffer.begin() + nr_bytes_written);
}


/*!
    @brief      Write benchmark results to standard output stream
*/
template<
    typename Benchmark>
void write_results(
    Benchmark const& benchmark)
{
    using Units = std::chrono::microseconds;

    auto const& time_interval = benchmark.time_interval();

    // auto meh = time_interval.durationf<Units>();

    std::cout
        << "Start at : " << to_iso_string(time_interval.start()) << "\n"
        << "Finish at: " << to_iso_string(time_interval.stop()) << "\n"
        << "Duration : " << time_interval.duration().count() << "\n"
        ;

    for(auto const& duration: benchmark.durations()) {
        std::cout
            << std::chrono::duration_cast<Units>(duration).count()
            << " microseconds" << std::endl;
    }
}

}  // benchmark
}  // lue

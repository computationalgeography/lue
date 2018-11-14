#pragma once
#include "lue/framework/benchmark/environment.hpp"
#include <nlohmann/json.hpp>
#include <array>
#include <cassert>


namespace lue {
namespace benchmark {

/*!
    @brief      Return a string representation of a time point

    The string is formatted according to ISO 8601: yyyy-mm-ddThh:mm:ss+zzzz.
    Example: 2018-10-11T15:55:59+0200
*/
inline std::string to_iso_string(
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


// /*!
//     @brief      Write benchmark results to standard output stream
// */
// template<
//     typename Benchmark>
// void write_results(
//     Benchmark const& benchmark)
// {
//     using Units = std::chrono::microseconds;
// 
//     auto const& time_interval = benchmark.time_interval();
// 
//     // auto meh = time_interval.duration<Units>();
// 
//     std::cout
//         << "Start at : " << to_iso_string(time_interval.start()) << "\n"
//         << "Finish at: " << to_iso_string(time_interval.stop()) << "\n"
//         << "Duration : " << time_interval.duration().count() << "\n"
//         ;
// 
//     for(auto const& timing: benchmark.timings()) {
//         std::cout
//             << to_iso_string(timing.time_interval().start()) << ": "
//             << std::chrono::duration_cast<Units>(
//                 timing.time_interval().duration()).count()
//             << " microseconds" << std::endl;
//     }
// }


/*!
    @brief      Format benchmark results as JSON
*/
template<
    typename Benchmark>
inline std::string format_as_json(
    Benchmark const& benchmark)
{
    using json = nlohmann::json;

    json j;

    // using Units = std::chrono::microseconds;

    auto const& time_interval = benchmark.time_interval();

    // auto meh = time_interval.duration<Units>();

    j["start"] = to_iso_string(time_interval.start());
    // j["stop"] = to_iso_string(time_interval.stop());
    j["duration"] = time_interval.duration().count();
    j["name"] = benchmark.name();
    j["system_name"] = benchmark.system_name();
    j["description"] = benchmark.description();

    auto const& environment = benchmark.environment();

    {
        auto environment_json = json::object();

        environment_json["nr_localities"] = environment.nr_localities();
        environment_json["nr_threads"] = environment.nr_threads();
        environment_json["work_size"] = environment.work_size();

        j["environment"] = environment_json;
    }

    auto a = json::array();

    for(auto const& timing: benchmark.timings()) {
        auto o = json::object();
        o["start"] = to_iso_string(timing.time_interval().start());
        o["duration"] = timing.time_interval().duration().count();

        a.emplace_back(o);
    }

    j["timings"] = a;

    return j.dump(4);
}

}  // benchmark
}  // lue

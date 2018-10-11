#pragma once
#include <iostream>


namespace lue {
namespace benchmark {

/*!
    @brief      Write benchmark results to standard output stream
*/
template<
    typename Benchmark>
void write_results(
    Benchmark const& benchmark)
{
    using Units = std::chrono::microseconds;

    for(auto const& duration: benchmark.durations()) {
        std::cout
            << std::chrono::duration_cast<Units>(duration).count()
            << " microseconds" << std::endl;
    }
}

}  // benchmark
}  // lue

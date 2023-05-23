#include "lue/qa/scalability/io.hpp"
#include <fstream>
#include <nlohmann/json.hpp>


namespace lue::qa {

    namespace {

        /*!
            @brief      Return a string representation of a time point

            The string is formatted according to ISO 8601: yyyy-mm-ddThh:mm:ss+zzzz.
            Example: 2018-10-11T15:55:59+0200
        */
        std::string to_iso_string(Stopwatch::SystemTimePoint const& time_point)
        {
            static std::size_t constexpr max_nr_characters{30};
            static std::array<char, max_nr_characters> buffer;

            auto const c_time_point = std::chrono::system_clock::to_time_t(time_point);
            auto const time = std::localtime(&c_time_point);

            auto const nr_bytes_written = std::strftime(buffer.data(), max_nr_characters, "%FT%T%z", time);
            assert(nr_bytes_written <= max_nr_characters);

            return std::string(buffer.begin(), buffer.begin() + nr_bytes_written);
        }


        std::string to_json(ArrayExperiment const& experiment)
        {
            using json = nlohmann::json;

            json j{};

            // The unit we want our durations to use
            using Unit = std::chrono::milliseconds;
            std::string unit = "millisecond";

            j["start"] = to_iso_string(experiment.start_time_point());
            j["duration"] = experiment.duration<Unit>().count();
            j["unit"] = unit;

            {
                json o = json::object();

                o["nr_workers"] = experiment.nr_workers();

                j["environment"] = o;
            }

            {
                json o = json::object();

                o["array_shape"] = experiment.array_shape();
                o["partition_shape"] = experiment.partition_shape();

                j["task"] = o;
            }

            {
                json a = json::array();

                for (Run const& run : experiment.runs())
                {
                    json o = json::object();

                    o["start"] = to_iso_string(run.start_time_point());
                    o["duration"] = run.duration<Unit>().count();

                    a.emplace_back(o);
                }

                j["timings"] = a;
            }

            /// if constexpr(!std::is_void_v<ResultT<Benchmark>>)
            /// {
            ///     j["results"] = experiment.results();
            /// }

            return j.dump(4);
        }

    }  // Anonymous namespace


    void save_results(ArrayExperiment const& experiment, std::string const& pathname)
    {
        // Create new json file and dump the information about this experiment
        std::ofstream stream{pathname};

        if(!stream)
        {
            throw std::runtime_error("File " + pathname + " could not be opened for saving results to");
        }

        stream << to_json(experiment);
    }

}  // namespace lue::qa

#include "lue/qa/scalability/instrument/io.hpp"
#include <fstream>
#include <nlohmann/json.hpp>


namespace lue::qa {
    namespace {

        /*!
            @brief      Return a string representation of a time point

            The string is formatted according to ISO 8601: yyyy-mm-ddThh:mm:ss+zzzz.
            Example: 2018-10-11T15:55:59+0200
        */
        auto to_iso_string(Stopwatch::SystemTimePoint const& time_point) -> std::string
        {
            static std::size_t constexpr max_nr_characters{30};
            static std::array<char, max_nr_characters> buffer;

            auto const c_time_point = std::chrono::system_clock::to_time_t(time_point);
            auto const* const time = std::localtime(&c_time_point);

            auto const nr_bytes_written = std::strftime(buffer.data(), max_nr_characters, "%FT%T%z", time);
            assert(nr_bytes_written <= max_nr_characters);

            return {buffer.begin(), buffer.begin() + nr_bytes_written};
        }


        auto to_json(ArrayExperiment const& experiment) -> std::string
        {
            using json = nlohmann::json;

            json root_object{};

            // The unit we want our durations to use
            using Unit = std::chrono::milliseconds;
            std::string unit = "millisecond";

            root_object["start"] = to_iso_string(experiment.start_time_point());
            root_object["duration"] = experiment.duration<Unit>().count();
            root_object["unit"] = unit;

            {
                json child_object = json::object();

                child_object["nr_workers"] = experiment.nr_workers();

                root_object["environment"] = child_object;
            }

            {
                json child_object = json::object();

                child_object["array_shape"] = experiment.array_shape();
                child_object["partition_shape"] = experiment.partition_shape();

                root_object["task"] = child_object;
            }

            {
                json child_array = json::array();

                for (Run const& run : experiment.runs())
                {
                    json child_object = json::object();

                    child_object["start"] = to_iso_string(run.start_time_point());
                    child_object["duration"] = run.duration<Unit>().count();

                    child_array.emplace_back(child_object);
                }

                root_object["timings"] = child_array;
            }

            /// if constexpr(!std::is_void_v<ResultT<Benchmark>>)
            /// {
            ///     root_object["results"] = experiment.results();
            /// }

            return root_object.dump(4);
        }

    }  // Anonymous namespace


    /*!
        @brief      Save the @a experiment results to a file called @a pathname
        @exception  std::runtime_error In case the file cannot be opened or is not writable
    */
    void save_results(ArrayExperiment const& experiment, std::string const& pathname)
    {
        // Create new json file and dump the information about this experiment
        std::ofstream stream{pathname};

        if (!stream)
        {
            throw std::runtime_error("File " + pathname + " could not be opened for saving results to");
        }

        stream << to_json(experiment);
    }

}  // namespace lue::qa

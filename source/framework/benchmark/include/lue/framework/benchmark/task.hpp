#pragma once
#include <cstdint>
#include <vector>


namespace lue {
    namespace benchmark {

        class Task
        {

            public:

                using Shape = std::vector<std::uint64_t>;

                Task(std::uint64_t nr_time_steps, Shape const& array_shape, Shape const& partition_shape);

                Task(Task const&) = default;

                Task(Task&&) = default;

                ~Task() = default;

                Task& operator=(Task const&) = default;

                Task& operator=(Task&&) = default;

                std::uint64_t nr_time_steps() const;

                std::size_t rank() const;

                std::uint64_t nr_elements() const;

                Shape const& array_shape() const;

                Shape const& partition_shape() const;

            private:

                std::uint64_t const _nr_time_steps;

                Shape const _array_shape;

                Shape const _partition_shape;
        };

    }  // namespace benchmark
}  // namespace lue

#pragma once
#include <ostream>


namespace lue {
    namespace utility {

        class ProgressIndicator
        {

            public:

                ProgressIndicator(std::ostream& stream, std::string const& label, size_t nr_tasks);

                ProgressIndicator(ProgressIndicator const&) = delete;

                ProgressIndicator(ProgressIndicator&&) = delete;

                ~ProgressIndicator();

                ProgressIndicator& operator=(ProgressIndicator const&) = delete;

                ProgressIndicator& operator=(ProgressIndicator&&) = delete;

                void update_progress(size_t task);

            private:

                void init_stream();

                void update_stream(std::string const& string);

                void finish_stream();

                std::ostream& _stream;

                std::string const _label;

                size_t const _nr_tasks;
        };

    }  // namespace utility
}  // namespace lue

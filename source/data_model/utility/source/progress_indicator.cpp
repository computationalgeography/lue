#include "lue/utility/progress_indicator.hpp"
#include <boost/format.hpp>


namespace lue {
    namespace utility {

        ProgressIndicator::ProgressIndicator(
            std::ostream& stream, std::string const& label, size_t const nr_tasks)

            :
            _stream(stream),
            _label(label),
            _nr_tasks(nr_tasks)

        {
            init_stream();
        }


        ProgressIndicator::~ProgressIndicator()
        {
            finish_stream();
        }


        void ProgressIndicator::init_stream()
        {
            _stream << _label << std::flush;
        }


        void ProgressIndicator::update_stream(std::string const& string)
        {
            _stream << "\r" << string << std::flush;
        }


        void ProgressIndicator::finish_stream()
        {
            _stream << std::endl;
        }


        /*!
            @brief      Update progress indicator
            @param      task Task currently being processed or just processed

            @a task should range from 1 to nr_tasks as passed into the constructor.
        */
        void ProgressIndicator::update_progress(size_t const task)
        {
            // TODO(KDJ) Update to use std::format
            auto string = boost::str(boost::format("%s: %3i%%") % _label % ((100 * task) / _nr_tasks));

            update_stream(string);
        }

    }  // namespace utility
}  // namespace lue

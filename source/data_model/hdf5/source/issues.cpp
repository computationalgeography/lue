#include "lue/hdf5/issues.hpp"


namespace lue {
    namespace hdf5 {

        /*!
            @brief      Return collection of errors
        */
        Errors const& Issues::errors() const
        {
            return _errors;
        }


        /*!
            @brief      Return collection of warnings
        */
        Warnings const& Issues::warnings() const
        {
            return _warnings;
        }


        /*!
            @brief      Add an issue to the collection of errors
        */
        void Issues::add_error(Identifier const& id, std::string const& message)
        {
            _errors.emplace_back(Issue(id, message));
        }


        /*!
            @brief      Add an issue to the collection of warnings
        */
        void Issues::add_warning(Identifier const& id, std::string const& message)
        {
            _warnings.emplace_back(Issue(id, message));
        }


        /*!
            @brief      Returns whether errors where found
        */
        bool Issues::errors_found() const
        {
            return !_errors.empty();
        }


        /*!
            @brief      Returns whether warnings where found
        */
        bool Issues::warnings_found() const
        {
            return !_warnings.empty();
        }

    }  // namespace hdf5
}  // namespace lue

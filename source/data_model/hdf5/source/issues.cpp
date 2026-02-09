#include "lue/hdf5/issues.hpp"


namespace lue::hdf5 {

    /*!
        @brief      Return collection of errors
    */
    auto Issues::errors() const -> Errors const&
    {
        return _errors;
    }


    /*!
        @brief      Return collection of warnings
    */
    auto Issues::warnings() const -> Warnings const&
    {
        return _warnings;
    }


    /*!
        @brief      Add an issue to the collection of errors
    */
    void Issues::add_error(Identifier const& id, std::string const& message)
    {
        _errors.emplace_back(id, message);
    }


    /*!
        @brief      Add an issue to the collection of warnings
    */
    void Issues::add_warning(Identifier const& id, std::string const& message)
    {
        _warnings.emplace_back(id, message);
    }


    /*!
        @brief      Returns whether errors where found
    */
    auto Issues::errors_found() const -> bool
    {
        return !_errors.empty();
    }


    /*!
        @brief      Returns whether warnings where found
    */
    auto Issues::warnings_found() const -> bool
    {
        return !_warnings.empty();
    }

}  // namespace lue::hdf5

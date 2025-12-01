#pragma once
#include "lue/hdf5/issue.hpp"


namespace lue::hdf5 {

    /*!
        @brief      Collection for storing errors
    */
    using Errors = std::vector<Issue>;


    /*!
        @brief      Collection for storing warnings
    */
    using Warnings = std::vector<Issue>;


    /*!
        @brief      Collections of issues (warnings and errors) found during
                    validation
        @sa         Issue
    */
    class LUE_HDF5_EXPORT Issues
    {

        public:

            Issues() = default;

            Issues(Issues const& other) = default;

            Issues(Issues&& other) = default;

            ~Issues() = default;

            auto operator=(Issues const& other) -> Issues& = default;

            auto operator=(Issues&& other) -> Issues& = default;

            auto errors() const -> Errors const&;

            auto warnings() const -> Warnings const&;

            void add_error(Identifier const& id, std::string const& message);

            void add_warning(Identifier const& id, std::string const& message);

            auto errors_found() const -> bool;

            auto warnings_found() const -> bool;

        private:

            //! Collection of errors
            Errors _errors;

            //! Collection of warnings
            Warnings _warnings;
    };

}  // namespace lue::hdf5

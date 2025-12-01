#pragma once
#include "lue/hdf5/identifier.hpp"
#include <string>


namespace lue::hdf5 {

    /*!
        @brief      Information about issue found during validation
    */
    class LUE_HDF5_EXPORT Issue
    {

        public:

            Issue(Identifier id, std::string message);

            Issue(Issue const& other) = default;

            Issue(Issue&& other) = default;

            ~Issue() = default;

            auto operator=(Issue const& other) -> Issue& = default;

            auto operator=(Issue&& other) -> Issue& = default;

            auto id() const -> Identifier const&;

            auto message() const -> std::string const&;

        private:

            //! Id of HDF5 object related to the issue
            Identifier _id;

            //! Message describing the issue
            std::string _message;
    };

}  // namespace lue::hdf5

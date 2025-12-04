#pragma once
#include "lue/hdf5/identifier.hpp"
#include <vector>


namespace lue::hdf5 {

    /*!
        @brief      This class provides a view on the HDF5 error stack
    */
    class ErrorStack
    {

        public:

            ErrorStack();

            explicit ErrorStack(Identifier id);

            ErrorStack(ErrorStack const& other) = default;

            ErrorStack(ErrorStack&& other) = default;

            ~ErrorStack();

            auto operator=(ErrorStack const& other) -> ErrorStack& = default;

            auto operator=(ErrorStack&& other) -> ErrorStack& = default;

            auto empty() const -> bool;

            void clear() const;

            auto messages() const -> std::vector<std::string>;

        private:

            Identifier _id;

            herr_t (*_original_error_handler)(hid_t, void*);

            void* _original_client_data;
    };

}  // namespace lue::hdf5

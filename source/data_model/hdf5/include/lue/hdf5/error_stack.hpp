#pragma once
#include "lue/hdf5/identifier.hpp"
#include <vector>


namespace lue {
    namespace hdf5 {

        /*!
            @brief      This class provides a view on the HDF5 error stack
        */
        class ErrorStack
        {

            public:

                ErrorStack();

                explicit ErrorStack(Identifier const& id);

                ErrorStack(ErrorStack const&) = default;

                ErrorStack(ErrorStack&&) = default;

                ~ErrorStack();

                ErrorStack& operator=(ErrorStack const&) = default;

                ErrorStack& operator=(ErrorStack&&) = default;

                bool empty() const;

                void clear() const;

                // void           add_message         (std::string const& message);

                std::vector<std::string> messages() const;

            private:

                Identifier _id;

                ::herr_t (*_original_error_handler)(::hid_t, void*);

                void* _original_client_data;
        };

    }  // namespace hdf5
}  // namespace lue

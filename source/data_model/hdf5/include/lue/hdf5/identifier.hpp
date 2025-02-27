#pragma once
#include "lue/hdf5/object_info.hpp"
#include <functional>
#include <string>


namespace lue::hdf5 {

    /*!
        @brief      This class represents an HDF5 identifier of an open
                    HDF5 object
        @sa         https://portal.hdfgroup.org/display/HDF5/Identifiers

        Scoping the identifier in this class ensures that the identifier is
        closed upon exiting the scope.

        Copies can be made. Only when the last copy goes out of scope will the
        identifier be closed.
    */
    class LUE_HDF5_EXPORT Identifier
    {

        public:

            /*!
                @brief      Type of function to call when the identifier must
                            be closed

                For example, when an HDF5 group is opened, you would pass H5Gclose
                as the close function into the constructor.
            */
            using Close = std::function<herr_t(hid_t)>;

            Identifier();

            Identifier(hid_t id, Close const& close);

            Identifier(Identifier const& other);

            Identifier(Identifier&& other) noexcept;

            ~Identifier() noexcept;

            Identifier& operator=(Identifier const& other);

            Identifier& operator=(Identifier&& other) noexcept;

            bool operator==(Identifier const& other) const;

            bool operator!=(Identifier const& other) const;

            bool is_valid() const;

            ::H5I_type_t type() const;

            void* object();

            int reference_count() const;

            // Allow explicit conversion to underlying hid_t. It is very
            // convenient, in this specific case.
            // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
            operator hid_t() const;

            std::string pathname() const;

            std::string name() const;

            ObjectInfo info() const;

            Identifier file_id() const;

        private:

            int increment_reference_count();

            void close_if_valid();

            void assert_invariant() const;

            //! HDF5 identifier
            hid_t _id;

            //! Function to call when the identifier must be closed
            Close _close;
    };

}  // namespace lue::hdf5

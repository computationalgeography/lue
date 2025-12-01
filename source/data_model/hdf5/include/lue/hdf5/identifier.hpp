#pragma once
#include "lue/hdf5/object_info.hpp"
#include <functional>
#include <string>


namespace lue::hdf5 {

    /*!
        @brief      This class represents an HDF5 identifier of an open HDF5 object
        @sa         https://portal.hdfgroup.org/display/HDF5/Identifiers

        Scoping the identifier in this class ensures that the identifier is closed upon exiting the scope.

        Copies can be made. Only when the last copy goes out of scope will the identifier be closed.
    */
    class LUE_HDF5_EXPORT Identifier
    {

        public:

            /*!
                @brief      Type of function to call when the identifier must be closed

                For example, when an HDF5 group is opened, you would pass H5Gclose as the close function into
                the constructor.
            */
            using Close = std::function<herr_t(hid_t)>;

            Identifier();

            Identifier(hid_t id, Close close);

            Identifier(Identifier const& other);

            Identifier(Identifier&& other) noexcept;

            ~Identifier() noexcept;

            auto operator=(Identifier const& other) -> Identifier&;

            auto operator=(Identifier&& other) noexcept -> Identifier&;

            auto operator==(Identifier const& other) const -> bool;

            auto operator!=(Identifier const& other) const -> bool;

            auto is_valid() const -> bool;

            auto type() const -> ::H5I_type_t;

            auto object() -> void*;

            auto reference_count() const -> int;

            // Allow explicit conversion to underlying hid_t. It is very convenient, in this specific case.
            // NOLINTNEXTLINE(google-explicit-constructor, hicpp-explicit-conversions)
            operator hid_t() const;

            auto pathname() const -> std::string;

            auto name() const -> std::string;

            auto info() const -> ObjectInfo;

            auto file_id() const -> Identifier;

        private:

            auto increment_reference_count() -> int;

            void close_if_valid();

            void assert_invariant() const;

            //! HDF5 identifier
            hid_t _id;

            //! Function to call when the identifier must be closed
            Close _close;
    };

}  // namespace lue::hdf5

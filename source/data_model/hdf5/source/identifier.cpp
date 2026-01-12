#include "lue/hdf5/identifier.hpp"
#include <cassert>
#include <cstring>
#include <stdexcept>
#include <type_traits>
#include <utility>


namespace lue::hdf5 {

    /*!
        @brief      Default construct an instance
        @sa         is_valid()

        The resulting instance will not be valid.
    */
    Identifier::Identifier():

        _id{-1},
        _close{nullptr}

    {
        assert(!is_valid());
    }


    /*!
        @brief      Construct an instance based on an HDF5 identifier and a close function
    */
    Identifier::Identifier(hid_t const id, Close close):

        _id{id},
        _close{std::move(close)}

    {
        assert_invariant();
    }


    /*!
        @brief      Copy construct an instance

        If the HDF5 identifier is valid, the reference count of the object it identifies will be incremented.
    */
    Identifier::Identifier(Identifier const& other):

        _id{other._id},
        _close{other._close}

    {
        if (is_valid())
        {
            increment_reference_count();
        }

        assert_invariant();
    }


    /*!
        @brief      Move construct an instance

        The resulting instance will be valid when @a other is valid, otherwise it will be invalid. Once the
        new instance is constructed, @a other will be in a valid state, but semantically invalid (not
        identifying an object anymore).
    */
    Identifier::Identifier(Identifier&& other) noexcept:

        _id{other._id},
        _close{std::move(other._close)}

    {
        other._id = -1;

        try
        {
            assert_invariant();
            assert(!other.is_valid());
        }
        catch (...)
        {
            // This should never happen
            assert(false);
        }
    }


    /*!
        @brief      Destruct the instance

        If necessary, the close function is called.
    */
    Identifier::~Identifier() noexcept
    {
        try
        {
            close_if_valid();
        }
        catch (...)
        {
            // This should never happen
            assert(false);
        }
    }


    /*!
        @brief      Copy-assign @a other to this instance

        If necessary, the close function is called on the currently layered HDF5 identifier before the
        assignment.
    */
    auto Identifier::operator=(Identifier const& other) -> Identifier&
    {
        // Copy-assign:
        // - Clean-up this instance
        // - Copy the other instance in

        if (&other != this)
        {
            close_if_valid();

            _id = other._id;

            if (is_valid())
            {
                increment_reference_count();
            }

            _close = other._close;

            assert_invariant();
        }

        return *this;
    }


    /*!
        @brief      Move-assign @a other to this instance

        If necessary, the close function is called on the currently layered HDF5 identifier before the
        assignment.
    */
    auto Identifier::operator=(Identifier&& other) noexcept -> Identifier&
    {
        // Move-assign:
        // - Clean-up this instance
        // - Move the other instance in

        try
        {
            close_if_valid();

            _id = other._id;
            other._id = -1;

            _close = std::move(other._close);

            assert_invariant();
            assert(!other.is_valid());
        }
        catch (...)
        {
            // This should never happen
            assert(false);
        }

        return *this;
    }


    /*!
        @brief      Return reference count of the object
        @exception  std::runtime_error In case the reference count cannot be retrieved. This happens when the
                    object is not valid.
    */
    auto Identifier::reference_count() const -> int
    {
        int const count{H5Iget_ref(_id)};

        if (count < 0)
        {
            throw std::runtime_error("Cannot retrieve object's reference count");
        }

        return count;
    }


    // NOLINTNEXTLINE(readability-make-member-function-const)
    auto Identifier::increment_reference_count() -> int
    {
        assert(is_valid());

        int const count{H5Iinc_ref(_id)};

        if (count < 0)
        {
            throw std::runtime_error("Cannot increment object's reference count");
        }

        return count;
    }


    /*!
        @brief      Close the HDF5 object pointed to by the instance

        Nothing will happen if the instance is not valid. Otherwise the layered close function which was
        passed upon construction is called. Typically, close functions will decrement the reference count of
        the object identified by the identifier and close the object when this count becomes zero.
    */
    void Identifier::close_if_valid()
    {
        if (is_valid())
        {
            assert(reference_count() > 0);
            _close(_id);
        }
    }


    // NOLINTBEGIN(readability-convert-member-functions-to-static)
    void Identifier::assert_invariant() const
    {
        assert(!(is_valid() && _close == nullptr));
    }
    // NOLINTEND(readability-convert-member-functions-to-static)


    /*!
        @brief      Return whether the identifier is valid
    */
    auto Identifier::is_valid() const -> bool
    {
        // Don't call assert_invariant here, since it uses is_valid in its
        // implementation...

        htri_t status{0};

        if (_id >= 0)
        {
            status = H5Iis_valid(_id);

            if (status < 0)
            {
                throw std::runtime_error("Cannot determine whether identifier is valid");
            }
        }

        return status > 0;
    }


    auto Identifier::type() const -> H5I_type_t
    {
        H5I_type_t const result{H5Iget_type(_id)};

        if (result == H5I_BADID)
        {
            throw std::runtime_error("Cannot determine type of object identifier");
        }

        return result;
    }


    // NOLINTNEXTLINE(readability-make-member-function-const)
    auto Identifier::object() -> void*
    {
        void* result{H5Iobject_verify(_id, type())};

        if (result == nullptr)
        {
            throw std::runtime_error("Cannot obtain pointer to object");
        }

        return result;
    }


    /*!
        @brief      Return the layered HDF5 identifier.
    */
    Identifier::operator hid_t() const
    {
        return _id;
    }


    /*!
        @brief      Return the pathname to the object
        @sa         name()

        There may be more than one pathname to an object. This function returns one of them. When possible, it
        is the one with which the object was opened.

        If the object identified by this identifier is an attribute, then the name of the object to which the
        attribute is attached is returned.
    */
    auto Identifier::pathname() const -> std::string
    {
        static_assert(
            std::is_same_v<std::string::value_type, char>, "expect std::string::value_type to be char");

        assert(is_valid());

        // Number of bytes, excluding \0
        ssize_t const nr_bytes{H5Iget_name(_id, nullptr, 0)};

        if (nr_bytes < 0)
        {
            throw std::runtime_error("Cannot retrieve name of object");
        }

        std::string result(nr_bytes, 'x');

        /* nr_bytes = */ H5Iget_name(_id, result.data(), nr_bytes + 1);

        return result;
    }


    /*!
        @brief      Return the name component of the pathname to the object
        @sa         pathname()

        The name is the part of the pathname after the last forward slash. If there is no forward slash in
        the pathname, name equals the pathname.

        An HDF5 object itself does not have a name. It has a unique ID within a file and at least one
        pathname pointing to it.
    */
    auto Identifier::name() const -> std::string
    {
        assert(is_valid());

        std::string const pathname{this->pathname()};
        std::size_t const idx{pathname.find_last_of('/')};

        return idx == std::string::npos ? pathname : pathname.substr(idx + 1);
    }


    auto Identifier::info() const -> ObjectInfo
    {
        assert(is_valid());

        return ObjectInfo{_id};
    }


    auto Identifier::file_id() const -> Identifier
    {
        assert(is_valid());

        hid_t const object_id{H5Iget_file_id(_id)};

        if (object_id < 0)
        {
            throw std::runtime_error("Cannot get file ID");
        }

        return Identifier{object_id, H5Fclose};
    }


    /*!
        @brief      Return whether two object identifiers are equal

        Two identifiers are considered equal if they are pointing to the same object in the HDF5 dataset.
    */
    auto Identifier::operator==(Identifier const& other) const -> bool
    {
        return info() == other.info();
    }


    /*!
        @brief      Return whether two object identifiers are not equal

        Two identifiers are considered equal if they are pointing to the same object in the HDF5 dataset.
    */
    auto Identifier::operator!=(Identifier const& other) const -> bool
    {
        return info() != other.info();
    }

}  // namespace lue::hdf5

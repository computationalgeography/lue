#include "lue/hdf5/primary_data_object.hpp"
#include <format>


namespace lue {
    namespace hdf5 {


        PrimaryDataObject::PrimaryDataObject(Identifier const& location, std::string const& name):

            PrimaryDataObject{Identifier{::H5Oopen(location, name.c_str(), H5P_DEFAULT), ::H5Oclose}}

        {
        }


        /*!
            @brief      Construct an object based on an identifier
            @warning    Whether or not the @a id passing in is valid must be
                        checked by calling code

            The identifier passed in is the id of the object itself; no object
            will be opened.
        */
        PrimaryDataObject::PrimaryDataObject(Identifier&& id):

            _id{std::move(id)},
            _attributes{_id}

        {
            // No assertions please. It is well possible that the id is not
            // valid. The calling code must check that.

            // assert(_id.is_valid());
        }


        /*!
            @brief      Return the object's identifier
        */
        Identifier const& PrimaryDataObject::id() const
        {
            return _id;
        }


        /*!
            @brief      Return the object's identifier
        */
        Identifier& PrimaryDataObject::id()
        {
            return _id;
        }


        /*!
            @brief      Return the object's attributes
        */
        Attributes const& PrimaryDataObject::attributes() const
        {
            return _attributes;
        }


        /*!
            @brief      Return the object's attributes
        */
        Attributes& PrimaryDataObject::attributes()
        {
            return _attributes;
        }


        /*!
            @brief      Return whether or not the object contains an attribute named
                        @a name
        */
        bool PrimaryDataObject::contains_attribute(std::string const& name) const
        {
            return _attributes.exists(name);
        }


        /*!
            @brief      Return whether two objects are equal
            @exception  std::runtime_error In case the metadata for the objects
                        passed in cannot be retrieved

            Two objects are considered equal if they are pointing to the same
            objects in the HDF5 dataset.
        */
        bool PrimaryDataObject::operator==(PrimaryDataObject const& other) const
        {
            return _id == other._id;
        }


        /*!
            @brief      Return whether two objects are not equal
            @exception  std::runtime_error In case the metadata for the objects
                        passed in cannot be retrieved

            Two objects are considered equal if they are pointing to the same
            object in the HDF5 dataset.
        */
        bool PrimaryDataObject::operator!=(PrimaryDataObject const& other) const
        {
            return _id != other._id;
        }

    }  // namespace hdf5
}  // namespace lue

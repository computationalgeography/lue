#pragma once
#include "lue/hdf5/attributes.hpp"
#include <cassert>


namespace lue {
    namespace hdf5 {

        /*!
            @brief      This class is a base class for the HDF5 primary data objects
                        (Dataset, Group, named datatype)
        */
        class LUE_HDF5_EXPORT PrimaryDataObject
        {

            public:

                PrimaryDataObject(Identifier const& location, std::string const& name);

                PrimaryDataObject(PrimaryDataObject const&) = default;

                PrimaryDataObject(PrimaryDataObject&&) = default;

                virtual ~PrimaryDataObject() = default;

                PrimaryDataObject& operator=(PrimaryDataObject const&) = default;

                PrimaryDataObject& operator=(PrimaryDataObject&&) = default;

                bool operator==(PrimaryDataObject const& other) const;

                bool operator!=(PrimaryDataObject const& other) const;

                Identifier const& id() const;

                Identifier& id();

                Attributes const& attributes() const;

                Attributes& attributes();

                template<typename T>
                T attribute(std::string const& name) const;

                bool contains_attribute(std::string const& name) const;

            protected:

                explicit PrimaryDataObject(Identifier&& id);

            private:

                //! Identifier
                Identifier _id;

                //! Attributes
                Attributes _attributes;
        };


        template<typename T>
        inline T PrimaryDataObject::attribute(std::string const& name) const
        {
            assert(contains_attribute(name));

            return _attributes.read<T>(name);
        }

    }  // namespace hdf5
}  // namespace lue

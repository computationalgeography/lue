#pragma once
#include "lue/hdf5/attributes.hpp"
#include <cassert>


namespace lue::hdf5 {

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

            auto operator=(PrimaryDataObject const&) -> PrimaryDataObject& = default;

            auto operator=(PrimaryDataObject&&) -> PrimaryDataObject& = default;

            auto operator==(PrimaryDataObject const& other) const -> bool;

            auto operator!=(PrimaryDataObject const& other) const -> bool;

            [[nodiscard]] auto id() const -> Identifier const&;

            auto id() -> Identifier&;

            [[nodiscard]] auto attributes() const -> Attributes const&;

            auto attributes() -> Attributes&;


            template<typename T>
            [[nodiscard]] auto attribute(std::string const& name) const -> T
            {
                assert(contains_attribute(name));

                return _attributes.read<T>(name);
            }


            [[nodiscard]] auto contains_attribute(std::string const& name) const -> bool;

        protected:

            explicit PrimaryDataObject(Identifier&& id);

        private:

            //! Identifier
            Identifier _id;

            //! Attributes
            Attributes _attributes;
    };

}  // namespace lue::hdf5

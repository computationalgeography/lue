#pragma once
#include "lue/hdf5/identifier.hpp"


namespace lue::hdf5 {

    /*!
        @brief      This class represents an HDF5 property list.
        @sa         [Property list
       interface](https://support.hdfgroup.org/HDF5/doc/RM/RM_H5P.html#FileAccessPropFuncs)
    */
    class PropertyList
    {

        public:

            PropertyList(PropertyList const& other) = default;

            PropertyList(PropertyList&& other) = default;

            virtual ~PropertyList() = default;

            auto operator=(PropertyList const& other) -> PropertyList& = default;

            auto operator=(PropertyList&& other) -> PropertyList& = default;

            auto id() const -> Identifier const&;

        protected:

            explicit PropertyList(Identifier&& property_list_id);

        private:

            Identifier _id;
    };

}  // namespace lue::hdf5

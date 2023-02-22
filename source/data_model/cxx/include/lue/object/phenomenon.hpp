#pragma once
#include "lue/info/identity/object_id.hpp"
#include "lue/object/property_sets.hpp"


namespace lue {
    namespace data_model {

        /*!
            @brief      Class for phenomenon information

            A phenomenon has two sets of property sets:
            - property sets: for storing property sets containing information
              for all active objects
            - collection property sets: for storing property sets containing
              information for the collection of objects as a whole
        */
        class Phenomenon: public hdf5::Group
        {

            public:

                Phenomenon(hdf5::Group const& parent, std::string const& name);

                explicit Phenomenon(hdf5::Group&& group);

                Phenomenon(Phenomenon const&) = default;

                Phenomenon(Phenomenon&&) = default;

                ~Phenomenon() override = default;

                Phenomenon& operator=(Phenomenon const&) = default;

                Phenomenon& operator=(Phenomenon&&) = default;

                std::string const& description() const;

                ObjectID const& object_id() const;

                ObjectID& object_id();

                PropertySets const& collection_property_sets() const;

                PropertySets& collection_property_sets();

                PropertySets const& property_sets() const;

                PropertySets& property_sets();

            private:

                std::string _description;

                ObjectID _object_id;

                //! Sets of properties of the collection of objects
                PropertySets _collection_property_sets;

                //! Sets of properties of the objects in the collection
                PropertySets _property_sets;
        };


        Phenomenon create_phenomenon(
            hdf5::Group& parent, std::string const& name, std::string const& description);

    }  // namespace data_model
}  // namespace lue

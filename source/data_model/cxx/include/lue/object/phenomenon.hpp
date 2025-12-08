#pragma once
#include "lue/info/identity/object_id.hpp"
#include "lue/object/property_sets.hpp"


namespace lue::data_model {

    /*!
        @brief      Class for phenomenon information

        A phenomenon has two sets of property sets:

        - property sets: for storing property sets containing information for all active objects
        - collection property sets: for storing property sets containing information for the collection
          of objects as a whole
    */
    class LUE_DATA_MODEL_EXPORT Phenomenon: public hdf5::Group
    {

        public:

            Phenomenon(hdf5::Group const& parent, std::string const& name);

            explicit Phenomenon(hdf5::Group&& group);

            Phenomenon(Phenomenon const& other) = default;

            Phenomenon(Phenomenon&& other) = default;

            ~Phenomenon() override = default;

            auto operator=(Phenomenon const& other) -> Phenomenon& = default;

            auto operator=(Phenomenon&& other) -> Phenomenon& = default;

            auto description() const -> std::string const&;

            auto object_id() const -> ObjectID const&;

            auto object_id() -> ObjectID&;

            auto collection_property_sets() const -> PropertySets const&;

            auto collection_property_sets() -> PropertySets&;

            auto property_sets() const -> PropertySets const&;

            auto property_sets() -> PropertySets&;

        private:

            std::string _description;

            ObjectID _object_id;

            //! Sets of properties of the collection of objects
            PropertySets _collection_property_sets;

            //! Sets of properties of the objects in the collection
            PropertySets _property_sets;
    };


    auto create_phenomenon(hdf5::Group& parent, std::string const& name, std::string const& description)
        -> Phenomenon;

}  // namespace lue::data_model

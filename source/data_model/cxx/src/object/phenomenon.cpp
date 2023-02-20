#include "lue/object/phenomenon.hpp"
#include "lue/core/tag.hpp"


namespace lue {
    namespace data_model {

        Phenomenon::Phenomenon(hdf5::Group const& parent, std::string const& name):

            hdf5::Group{parent, name},
            _description{
                attributes().exists(description_tag) ? attributes().read<std::string>(description_tag) : ""},
            _object_id{*this},
            _collection_property_sets{*this, collection_property_sets_tag},
            _property_sets{*this, property_sets_tag}

        {
        }


        Phenomenon::Phenomenon(hdf5::Group&& group):

            hdf5::Group{std::move(group)},
            _description{
                attributes().exists(description_tag) ? attributes().read<std::string>(description_tag) : ""},
            _object_id{*this},
            _collection_property_sets{*this, collection_property_sets_tag},
            _property_sets{*this, property_sets_tag}

        {
        }


        std::string const& Phenomenon::description() const
        {
            return _description;
        }


        /*!
            @brief      Return ID instance storing object IDs
        */
        ObjectID const& Phenomenon::object_id() const
        {
            return _object_id;
        }


        /*!
            @brief      Return ID instance storing object IDs
        */
        ObjectID& Phenomenon::object_id()
        {
            return _object_id;
        }


        /*!
            @brief      Return collection of property sets for storing information
                        for the collection of objects as a whole
            @param      .
            @return     .
            @exception  .
        */
        PropertySets const& Phenomenon::collection_property_sets() const
        {
            return _collection_property_sets;
        }


        PropertySets& Phenomenon::collection_property_sets()
        {
            return _collection_property_sets;
        }


        PropertySets const& Phenomenon::property_sets() const
        {
            return _property_sets;
        }


        PropertySets& Phenomenon::property_sets()
        {
            return _property_sets;
        }


        Phenomenon create_phenomenon(
            hdf5::Group& parent, std::string const& name, std::string const& description)
        {
            if (hdf5::group_exists(parent, name))
            {
                throw std::runtime_error(
                    fmt::format("Phenomenon {} already exists at {}", name, parent.id().pathname()));
            }

            hdf5::Group group{hdf5::create_group(parent, name)};

            // Possibly empty
            if (!description.empty())
            {
                group.attributes().write(description_tag, description);
            }

            create_object_id(group);
            create_property_sets(group, collection_property_sets_tag);
            create_property_sets(group, property_sets_tag);

            return Phenomenon{std::move(group)};
        }

    }  // namespace data_model
}  // namespace lue

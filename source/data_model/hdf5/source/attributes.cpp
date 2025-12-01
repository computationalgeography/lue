#include "lue/hdf5/attributes.hpp"


namespace lue::hdf5 {

    /*!
        @brief      Construct collection based on @a id
        @param      id Id of object containing attributes
    */
    Attributes::Attributes(Identifier const& id):

        _id{id}

    {
    }


    /*!
        @brief      Add a new attribute to the set
        @param      name Name of attribute
        @param      datatype In-file datatype of the value
        @param      dataspace Dataspace of the value
    */
    auto Attributes::add(std::string const& name, Datatype const& datatype, Dataspace const& dataspace)
        -> Attribute
    {
        return create_attribute(_id, name, datatype, dataspace);
    }


    /*!
        @brief      Return whether an attribute named @a name is present
                    in the layered object
    */
    auto Attributes::exists(std::string const& name) const -> bool
    {
        return H5Aexists(_id, name.c_str()) > 0;
    }


    /*!
        @brief      Return the attribute named @a name
    */
    auto Attributes::attribute(std::string const& name) const -> Attribute
    {
        assert(_id.is_valid());
        assert(exists(name));

        return {_id, name};
    }


    /*!
        @brief      Return the datatype of the attribute named @a name
    */
    auto Attributes::datatype(std::string const& name) const -> Datatype
    {
        return attribute(name).datatype();
    }

}  // namespace lue::hdf5

#include "lue/hdf5/attribute.hpp"


namespace lue::hdf5 {

    /*!
        @brief      Construct instance based on @a id
        @param      id Identifier of attribute
    */
    Attribute::Attribute(Identifier&& id):

        _id{std::move(id)},
        _datatype{::H5Aget_type(_id)},
        _dataspace{::H5Aget_space(_id)}

    {
    }


    /*!
        @brief      Open attribute named @a name
        @param      location Location of attribute
        @param      name Name of attribute
        @exception  std::runtime_error In case attribute cannot be opened
    */
    Attribute::Attribute(Identifier const& location, std::string const& name):

        _id(::H5Aopen(location, name.c_str(), H5P_DEFAULT), ::H5Aclose),
        _datatype{::H5Aget_type(_id)},
        _dataspace{::H5Aget_space(_id)}

    {
    }


    /*!
        @brief      Return the id of the attribute
    */
    auto Attribute::id() const -> Identifier const&
    {
        return _id;
    }


    /*!
        @brief      Return the datatype of the attribute
    */
    auto Attribute::datatype() const -> Datatype const&
    {
        return _datatype;
    }


    /*!
        @brief      Return the dataspace of the attribute
    */
    auto Attribute::dataspace() const -> Dataspace const&
    {
        return _dataspace;
    }


    /*!
        @brief      Create attribute
        @param      location Location
        @param      name Name
        @param      datatype Datatype
        @param      dataspace Dataspace
        @exception  std::runtime_error In case attribute cannot be created
    */
    auto create_attribute(
        Identifier const& location,
        std::string const& name,
        Datatype const& datatype,
        Dataspace const& dataspace) -> Attribute
    {
        // TODO This configures attribute names to be encoded in utf-8. Refactor, etc
        // auto plist = H5Pcreate(H5P_ATTRIBUTE_CREATE);
        // H5Pset_char_encoding(plist, H5T_CSET_UTF8);

        Identifier id{
            ::H5Acreate(location, name.c_str(), datatype.id(), dataspace.id(), H5P_DEFAULT, H5P_DEFAULT),
            ::H5Aclose};

        if (!id.is_valid())
        {
            throw std::runtime_error("Cannot create attribute");
        }

        return id;
    }

}  // namespace lue::hdf5

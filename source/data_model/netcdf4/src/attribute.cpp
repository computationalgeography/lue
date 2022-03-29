#include "lue/data_model/netcdf4/attribute.hpp"
#include <fmt/format.h>


namespace lue::data_model::netcdf4 {

    namespace {

        std::string attribute_name(
            Identifier const& group_id,
            Identifier const& variable_id,
            Identifier const& attribute_id)
        {
            static char name[NC_MAX_NAME];
            int const status{::nc_inq_attname(group_id, variable_id, attribute_id, name)};

            if(status != NC_NOERR)
            {
                throw std::runtime_error("Cannot obtain name of attribute");
            }

            // This assumes the name is null-terminated
            return std::string{name};
        }


        std::string attribute_name(
            Identifier const& group_id,
            Identifier const& attribute_id)
        {
            return attribute_name(group_id, NC_GLOBAL, attribute_id);
        }


        nc_type attribute_type(
            Identifier const& group_id,
            Identifier const& variable_id,
            std::string const& name)
        {
            nc_type data_type;
            int const status{::nc_inq_atttype(group_id, variable_id, name.c_str(), &data_type)};

            if(status != NC_NOERR)
            {
                throw std::runtime_error(fmt::format("Cannot obtain data type of attribute {}", name));
            }

            return data_type;
        }


        nc_type attribute_type(
            Identifier const& group_id,
            std::string const& name)
        {
            return attribute_type(group_id, NC_GLOBAL, name);
        }


        size_t attribute_length(
            Identifier const& group_id,
            Identifier const& variable_id,
            std::string const& name)
        {
            size_t length;
            int const status{::nc_inq_attlen(group_id, variable_id, name.c_str(), &length)};

            if(status != NC_NOERR)
            {
                throw std::runtime_error(fmt::format("Cannot obtain length of attribute {}", name));
            }

            return length;
        }


        size_t attribute_length(
            Identifier const& group_id,
            std::string const& name)
        {
            return attribute_length(group_id, NC_GLOBAL, name);
        }

    }  // Anonymous namespace


    Attribute::Attribute(
        Identifier const& group_id,
        Identifier const& attribute_id):

        _id{attribute_id}

    {
        _name = attribute_name(group_id, attribute_id);
        _data_type = attribute_type(group_id, _name);
        _length = attribute_length(group_id, _name);
    }


    Attribute::Attribute(
        Identifier const& group_id,
        Identifier const& variable_id,
        Identifier const& attribute_id):

        _id{attribute_id}

    {
        _name = attribute_name(group_id, variable_id, attribute_id);
        _data_type = attribute_type(group_id, variable_id, _name);
        _length = attribute_length(group_id, variable_id, _name);
    }


    std::string const& Attribute::name() const
    {
        return _name;
    }


    nc_type Attribute::data_type() const
    {

        // NC_BYTE   -> int8_t
        // NC_SHORT  -> int16_t
        // NC_INT    -> int32_t
        // NC_INT64  -> int64_t
        //
        // NC_UBYTE  -> uint8_t
        // NC_USHORT -> uint16_t
        // NC_UINT   -> uint32_t
        // NC_UINT64 -> uint64_t
        //
        // NC_FLOAT  -> float32
        // NC_DOUBLE -> float64
        //
        // NC_STRING -> string
        // NC_CHAR   -> ISO/ASCII character
        return _data_type;
    }


    // std::string attribute(
    //     Identifier const& group,
    //     std::string const& name)
    // {
    //     std::string value(attribute_length(group, name), ' ');

    //     int const status{::nc_get_att_text(group, NC_GLOBAL, name.c_str(), value.data())};

    //     if(status != NC_NOERR)
    //     {
    //         throw std::runtime_error(fmt::format("Cannot get value of attribute {}", name));
    //     }

    //     // The handling of NULL terminators is not specified by NetCDF. Remove it if it was
    //     // present.
    //     if(!value.empty() && value.back() == '\0')
    //     {
    //         value.resize(value.size() - 1);
    //     }

    //     return value;
    // }


    // template<>
    // std::vector<int> attribute<int>(
    //     Identifier const& group,
    //     std::string const& name)
    // {
    //     std::vector<int> value(attribute_length(group, name));

    //     int const status{::nc_get_att_int(group, NC_GLOBAL, name.c_str(), value.data())};

    //     if(status != NC_NOERR)
    //     {
    //         throw std::runtime_error(fmt::format("Cannot get value of attribute {}", name));
    //     }

    //     return value;
    // }

}  // namespace lue::data_model::netcdf4

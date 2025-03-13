#pragma once
#include "lue/hdf5/attribute.hpp"


namespace lue {
    namespace hdf5 {

        /*!
            @brief      This class represents the collection of attributes attached
                        to an object
        */
        class LUE_HDF5_EXPORT Attributes
        {

            public:

                explicit Attributes(Identifier const& id);

                Attribute add(
                    std::string const& name, Datatype const& file_datatype, Dataspace const& dataspace);

                template<typename T>
                void write(std::string const& name, T const& value);

                template<typename T>
                T read(std::string const& name) const;

                bool exists(std::string const& name) const;

                Attribute attribute(std::string const& name) const;

                Datatype datatype(std::string const& name) const;

            private:

                //! Identifier of object owning the attributes
                Identifier _id;
        };


        /*!
            @brief      Return value of attribute @a name
            @sa         Attribute::read(std::string const&)
        */
        template<typename T>
        inline T Attributes::read(std::string const& name) const
        {
            return attribute(name).read<T>();
        }


        /*!
            @brief      Store @a value of attribute @a name

            In case no attribute named @a name exists, it is created.
        */
        template<typename T>
        inline void Attributes::write(std::string const& name, T const& value)
        {
            assert(_id.is_valid());

            if (!exists(name))
            {
                create_attribute(_id, name, value);
            }
            else
            {
                attribute(name).write<T>(value);
            }
        }

    }  // namespace hdf5
}  // namespace lue

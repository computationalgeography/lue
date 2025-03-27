#pragma once
#include "lue/hdf5/attribute.hpp"


namespace lue::hdf5 {

    /*!
        @brief      This class represents the collection of attributes attached
                    to an object
    */
    class LUE_HDF5_EXPORT Attributes
    {

        public:

            explicit Attributes(Identifier const& id);

            auto add(std::string const& name, Datatype const& file_datatype, Dataspace const& dataspace)
                -> Attribute;


            /*!
                @brief      Store @a value of attribute @a name

                In case no attribute named @a name exists, it is created.
            */
            template<typename T>
            void write(std::string const& name, T const& value)
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


            /*!
                @brief      Return value of attribute @a name
                @sa         Attribute::read(std::string const&)
            */
            template<typename T>
            auto read(std::string const& name) const -> T
            {
                return attribute(name).read<T>();
            }


            [[nodiscard]] auto exists(std::string const& name) const -> bool;

            [[nodiscard]] auto attribute(std::string const& name) const -> Attribute;

            [[nodiscard]] auto datatype(std::string const& name) const -> Datatype;

        private:

            //! Identifier of object owning the attributes
            Identifier _id;
    };

}  // namespace lue::hdf5

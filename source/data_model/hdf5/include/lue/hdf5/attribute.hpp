#pragma once
#include "lue/hdf5/dataspace.hpp"
#include "lue/hdf5/datatype.hpp"
#include "lue/hdf5/datatype_traits.hpp"
#include <cassert>
#include <stdexcept>
#include <type_traits>


namespace lue::hdf5 {
    namespace detail {

        template<typename T>
            requires(!std::is_base_of_v<std::vector<::hsize_t>, T>)
        void read_attribute(
            Identifier const& id,
            Datatype const& datatype,
            [[maybe_unused]] Dataspace const& dataspace,
            T& value)
        {
            herr_t const status{H5Aread(id, datatype.id(), &value)};

            if (status < 0)
            {
                throw std::runtime_error("Cannot read attribute");
            }
        }


        template<>
        inline void read_attribute<std::string>(
            Identifier const& id,
            Datatype const& datatype,
            [[maybe_unused]] Dataspace const& dataspace,
            std::string& value)
        {
            static_assert(
                std::is_same_v<std::string::value_type, char>, "expect std::string::value_type to be char");

            assert(datatype.encoding() == H5T_CSET_UTF8);

            std::size_t const nr_bytes{datatype.size()};

            value.resize(nr_bytes, 'x');

            herr_t const status{H5Aread(id, datatype.id(), value.data())};
            assert(value.size() == nr_bytes);

            if (status < 0)
            {
                throw std::runtime_error("Cannot read attribute");
            }
        }


        template<typename T>
        void read_attribute(
            Identifier const& id, Datatype const& datatype, Dataspace const& dataspace, std::vector<T>& value)
        {
            hssize_t const nr_elements{dataspace.nr_elements()};

            value.resize(nr_elements);

            herr_t const status{H5Aread(id, datatype.id(), value.data())};
            assert(value.size() == static_cast<std::size_t>(nr_elements));

            if (status < 0)
            {
                throw std::runtime_error("Cannot read attribute");
            }
        }


        template<typename T>
            requires(!std::is_base_of_v<std::vector<::hsize_t>, T>)
        void write_attribute(Identifier const& id, Datatype const& datatype, T const& value)
        {
            herr_t const status{H5Awrite(id, datatype.id(), &value)};

            if (status < 0)
            {
                throw std::runtime_error("Cannot write attribute");
            }
        }


        template<>
        inline void write_attribute<std::string>(
            Identifier const& id, Datatype const& datatype, std::string const& value)
        {
            herr_t const status{H5Awrite(id, datatype.id(), value.c_str())};

            if (status < 0)
            {
                throw std::runtime_error("Cannot write attribute");
            }
        }


        template<typename T>
        void write_attribute(Identifier const& id, Datatype const& datatype, std::vector<T> const& value)
        {
            herr_t const status{H5Awrite(id, datatype.id(), value.data())};

            if (status < 0)
            {
                throw std::runtime_error("Cannot write attribute");
            }
        }

    }  // namespace detail


    /*!
        @brief      This class represents an attribute
    */
    class LUE_HDF5_EXPORT Attribute
    {

        public:

            Attribute(Identifier const& location, std::string const& name);

            Attribute(Attribute const& other) = default;

            Attribute(Attribute&& other) = default;

            Attribute(Identifier&& id);

            ~Attribute() = default;

            auto operator=(Attribute const& other) -> Attribute& = default;

            auto operator=(Attribute&& other) -> Attribute& = default;

            auto id() const -> Identifier const&;

            auto datatype() const -> Datatype const&;

            auto dataspace() const -> Dataspace const&;


            /*!
                @brief      Write attribute value
                @tparam     T Type of value to write
                @exception  std::runtime_error In case the attribute cannot be written
                @warning    It is assumed that @a T corresponds with the attribute's
                            datatype
            */
            template<typename T>
            void write(T const& value)
            {
                detail::write_attribute(id(), datatype(), value);
            }


            template<typename T>
            void write(Datatype const& memory_datatype, T const& value)
            {
                detail::write_attribute(id(), memory_datatype, value);
            }


            /*!
                @brief      Read attribute value
                @tparam     T Type of value to read
                @exception  std::runtime_error In case the attribute cannot be read
                @warning    It is assumed that @a T corresponds with the attribute's
                            datatype
            */
            template<typename T>
            auto read() const -> T
            {
                T value;
                detail::read_attribute(id(), datatype(), dataspace(), value);

                return value;
            }


            template<typename T>
            void read(Datatype const& memory_datatype, T& value)
            {
                detail::read_attribute(id(), memory_datatype, dataspace(), value);
            }

        private:

            //! Identifier of attribute
            Identifier _id;

            //! Datatype of attribute
            Datatype _datatype;

            //! Dataspace of attribute
            Dataspace _dataspace;
    };


    LUE_HDF5_EXPORT auto create_attribute(
        Identifier const& location,
        std::string const& name,
        Datatype const& datatype,
        Dataspace const& dataspace) -> Attribute;


    /*!
        @brief      Create attribute and initialize its value
        @param      location Location
        @param      name Name
        @param      value Initial value
        @sa         create_attribute(Identifier const&, std::string const&,
                    Datatype const&, Dataspace const&)

        This overload is enabled for all types that don't enherit from
        std::vector<hsize_t>. This is done to let the compiler pick the
        overload for std::vector<T> for Shape, for example.
    */
    template<typename T>
        requires(!std::is_base_of_v<std::vector<hsize_t>, T>)
    auto create_attribute(Identifier const& location, std::string const& name, T const& value) -> Attribute
    {
        Datatype const datatype{NativeDatatypeTraits<T>::type_id()};
        Dataspace const dataspace{H5S_SCALAR};
        Attribute attribute{create_attribute(location, name, datatype, dataspace)};

        attribute.write(value);

        return attribute;
    }


    template<>
    inline auto create_attribute(
        Identifier const& location, std::string const& name, std::string const& value) -> Attribute
    {
        Datatype const datatype{create_datatype(value.size())};
        Dataspace const dataspace{Dataspace{H5S_SCALAR}};
        Attribute attribute{create_attribute(location, name, datatype, dataspace)};

        attribute.write(value);

        return attribute;
    }


    template<typename T>
    auto create_attribute(Identifier const& location, std::string const& name, std::vector<T> const& value)
        -> Attribute
    {
        Datatype const datatype{Datatype{NativeDatatypeTraits<T>::type_id()}};
        Dataspace const dataspace{create_dataspace(Shape{value.size()})};
        Attribute attribute{create_attribute(location, name, datatype, dataspace)};

        attribute.write(value);

        return attribute;
    }

}  // namespace lue::hdf5

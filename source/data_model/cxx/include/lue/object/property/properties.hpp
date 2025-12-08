#pragma once
#include "lue/object/property/different_shape/constant_shape/properties.hpp"
#include "lue/object/property/different_shape/properties.hpp"
#include "lue/object/property/different_shape/variable_shape/properties.hpp"
#include "lue/object/property/same_shape/constant_shape/properties.hpp"
#include "lue/object/property/same_shape/properties.hpp"
#include "lue/object/property/same_shape/variable_shape/properties.hpp"


namespace lue::data_model {

    class LUE_DATA_MODEL_EXPORT Properties: public hdf5::Group
    {

        public:

            explicit Properties(hdf5::Group const& parent);

            explicit Properties(hdf5::Group&& group);

            Properties(Properties const& other) = default;

            Properties(Properties&& other) = default;

            ~Properties() override = default;

            auto operator=(Properties const& other) -> Properties& = default;

            auto operator=(Properties&& other) -> Properties& = default;

            auto empty() const -> bool;

            auto size() const -> std::size_t;

            // std::vector<std::string>
            //                names               () const;

            auto contains(std::string const& name) const -> bool;

            auto shape_per_object(std::string const& name) const -> ShapePerObject;

            auto value_variability(std::string const& name) const -> ValueVariability;

            auto shape_variability(std::string const& name) const -> ShapeVariability;

            auto collection_group(std::string const& name) -> hdf5::Group&;

            template<typename Collection>
            auto names() const -> std::vector<std::string>;

            template<typename T>
            auto collection() const -> T const&;

            template<typename T>
            auto collection() -> T&;

            auto add(
                std::string const& name, hdf5::Datatype const& datatype, std::string const& description = "")
                -> same_shape::Property&;

            auto add(
                std::string const& name,
                hdf5::Datatype const& datatype,
                hdf5::Shape const& shape,
                std::string const& description = "") -> same_shape::Property&;

            auto add(
                std::string const& name,
                hdf5::Datatype const& datatype,
                Rank rank,
                std::string const& description = "") -> different_shape::Property&;

            template<typename Property>
            auto add(
                std::string const& name, hdf5::Datatype const& datatype, std::string const& description = "")
                -> Property&;

            template<typename Property>
            auto add(
                std::string const& name,
                hdf5::Datatype const& datatype,
                hdf5::Shape const& shape,
                std::string const& description = "") -> Property&;

            template<typename Property>
            auto add(
                std::string const& name,
                hdf5::Datatype const& datatype,
                Rank rank,
                std::string const& description = "") -> Property&;

        private:

            void verify_property_does_not_exist(std::string const& name) const;

            same_shape::Properties _same_shape_properties;

            same_shape::constant_shape::Properties _same_shape_constant_shape_properties;

            same_shape::variable_shape::Properties _same_shape_variable_shape_properties;

            different_shape::Properties _different_shape_properties;

            different_shape::constant_shape::Properties _different_shape_constant_shape_properties;

            different_shape::variable_shape::Properties _different_shape_variable_shape_properties;
    };


    auto create_properties(hdf5::Group& parent) -> Properties;


#define COLLECTION(type, member)                                                                             \
    template<>                                                                                               \
    inline auto Properties::collection<type>() const -> type const&                                          \
    {                                                                                                        \
        return member;                                                                                       \
    }                                                                                                        \
                                                                                                             \
                                                                                                             \
    template<>                                                                                               \
    inline auto Properties::collection<type>() -> type&                                                      \
    {                                                                                                        \
        return member;                                                                                       \
    }


    COLLECTION(same_shape::Properties, _same_shape_properties)
    COLLECTION(same_shape::constant_shape::Properties, _same_shape_constant_shape_properties)
    COLLECTION(same_shape::variable_shape::Properties, _same_shape_variable_shape_properties)
    COLLECTION(different_shape::Properties, _different_shape_properties)
    COLLECTION(different_shape::constant_shape::Properties, _different_shape_constant_shape_properties)
    COLLECTION(different_shape::variable_shape::Properties, _different_shape_variable_shape_properties)

#undef COLLECTION


    template<typename Collection>
    auto Properties::names() const -> std::vector<std::string>
    {
        return collection<Collection>().names();
    }


    template<>
    inline auto Properties::add<same_shape::Property>(
        std::string const& name, hdf5::Datatype const& datatype, std::string const& description)
        -> same_shape::Property&
    {
        verify_property_does_not_exist(name);

        return _same_shape_properties.add(name, datatype, description);
    }


    template<>
    inline auto Properties::add<same_shape::Property>(
        std::string const& name,
        hdf5::Datatype const& datatype,
        hdf5::Shape const& shape,
        std::string const& description) -> same_shape::Property&
    {
        verify_property_does_not_exist(name);

        return _same_shape_properties.add(name, datatype, shape, description);
    }


    template<>
    inline auto Properties::add<same_shape::constant_shape::Property>(
        std::string const& name, hdf5::Datatype const& datatype, std::string const& description)
        -> same_shape::constant_shape::Property&
    {
        verify_property_does_not_exist(name);

        return _same_shape_constant_shape_properties.add(name, datatype, description);
    }


    template<>
    inline auto Properties::add<same_shape::constant_shape::Property>(
        std::string const& name,
        hdf5::Datatype const& datatype,
        hdf5::Shape const& shape,
        std::string const& description) -> same_shape::constant_shape::Property&
    {
        verify_property_does_not_exist(name);

        return _same_shape_constant_shape_properties.add(name, datatype, shape, description);
    }


    template<>
    inline auto Properties::add<same_shape::variable_shape::Property>(
        std::string const& name,
        hdf5::Datatype const& datatype,
        Rank const rank,
        std::string const& description) -> same_shape::variable_shape::Property&
    {
        verify_property_does_not_exist(name);

        return _same_shape_variable_shape_properties.add(name, datatype, rank, description);
    }


    template<>
    inline auto Properties::add<different_shape::constant_shape::Property>(
        std::string const& name,
        hdf5::Datatype const& datatype,
        Rank const rank,
        std::string const& description) -> different_shape::constant_shape::Property&
    {
        verify_property_does_not_exist(name);

        return _different_shape_constant_shape_properties.add(name, datatype, rank, description);
    }


    template<>
    inline auto Properties::add<different_shape::variable_shape::Property>(
        std::string const& name,
        hdf5::Datatype const& datatype,
        Rank const rank,
        std::string const& description) -> different_shape::variable_shape::Property&
    {
        verify_property_does_not_exist(name);

        return _different_shape_variable_shape_properties.add(name, datatype, rank, description);
    }

}  // namespace lue::data_model

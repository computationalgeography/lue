#include "lue/object/property/properties.hpp"
#include "lue/core/tag.hpp"
#include <boost/optional.hpp>
#include <format>
#include <functional>


namespace lue::data_model {

    Properties::Properties(hdf5::Group const& parent):

        hdf5::Group{parent, properties_tag},
        _same_shape_properties{*this},
        _same_shape_constant_shape_properties{*this},
        _same_shape_variable_shape_properties{*this},
        _different_shape_properties{*this},
        _different_shape_constant_shape_properties{*this},
        _different_shape_variable_shape_properties{*this}

    {
    }


    Properties::Properties(hdf5::Group&& group):

        hdf5::Group{std::move(group)},
        _same_shape_properties{*this},
        _same_shape_constant_shape_properties{*this},
        _same_shape_variable_shape_properties{*this},
        _different_shape_properties{*this},
        _different_shape_constant_shape_properties{*this},
        _different_shape_variable_shape_properties{*this}

    {
    }


    auto Properties::empty() const -> bool
    {
        return size() == 0;
    }


    auto Properties::size() const -> std::size_t
    {
        return _same_shape_properties.size() + _same_shape_constant_shape_properties.size() +
               _same_shape_variable_shape_properties.size() + _different_shape_properties.size() +
               _different_shape_constant_shape_properties.size() +
               _different_shape_variable_shape_properties.size();
    }


    // std::vector<std::string> Properties::names() const
    // {
    //     std::vector<std::string> names;
    //
    //     {
    //         auto const names_ = _same_shape_properties.names();
    //         names.insert(names.end(), names_.begin(), names_.end());
    //     }
    //
    //
    //     // _same_shape_constant_shape_properties.size() +
    //     // _same_shape_variable_shape_properties.size() +
    //     // _different_shape_properties.size() +
    //     // _different_shape_constant_shape_properties.size() +
    //     // _different_shape_variable_shape_properties.size()
    //
    //     return names;
    // }


    auto Properties::contains(std::string const& name) const -> bool
    {
        return _same_shape_properties.contains(name) ||
               _same_shape_constant_shape_properties.contains(name) ||
               _same_shape_variable_shape_properties.contains(name) ||
               _different_shape_properties.contains(name) ||
               _different_shape_constant_shape_properties.contains(name) ||
               _different_shape_variable_shape_properties.contains(name);
    }


    auto Properties::shape_per_object(std::string const& name) const -> ShapePerObject
    {
        boost::optional<ShapePerObject> result;

        if (_same_shape_properties.contains(name) || _same_shape_constant_shape_properties.contains(name) ||
            _same_shape_variable_shape_properties.contains(name))
        {
            result = ShapePerObject::same;
        }
        else if (
            _different_shape_properties.contains(name) ||
            _different_shape_constant_shape_properties.contains(name) ||
            _different_shape_variable_shape_properties.contains(name))
        {
            result = ShapePerObject::different;
        }

        if (!result)
        {
            throw std::runtime_error(std::format("Property named {} does not exist", name));
        }

        return *result;
    }


    auto Properties::value_variability(std::string const& name) const -> ValueVariability
    {
        boost::optional<ValueVariability> result;

        if (_same_shape_properties.contains(name) || _different_shape_properties.contains(name))
        {
            result = ValueVariability::constant;
        }
        else if (
            _same_shape_constant_shape_properties.contains(name) ||
            _same_shape_variable_shape_properties.contains(name) ||
            _different_shape_constant_shape_properties.contains(name) ||
            _different_shape_variable_shape_properties.contains(name))
        {
            result = ValueVariability::variable;
        }

        if (!result)
        {
            throw std::runtime_error(std::format("Property named {} does not exist", name));
        }

        return *result;
    }


    auto Properties::shape_variability(std::string const& name) const -> ShapeVariability
    {
        boost::optional<ShapeVariability> result;

        if (_same_shape_constant_shape_properties.contains(name) ||
            _different_shape_constant_shape_properties.contains(name))
        {
            result = ShapeVariability::constant;
        }
        else if (
            _same_shape_variable_shape_properties.contains(name) ||
            _different_shape_variable_shape_properties.contains(name))
        {
            result = ShapeVariability::variable;
        }

        if (!result)
        {
            throw std::runtime_error(
                std::format("Property named {} does not exist or does not vary through time", name));
        }

        return *result;
    }


    /*!
        @brief      Return the collection group containing the property named @a name
    */
    auto Properties::collection_group(std::string const& name) -> hdf5::Group&
    {
        hdf5::Group* result = nullptr;

        if (_same_shape_properties.contains(name))
        {
            result = &_same_shape_properties;
        }
        else if (_same_shape_constant_shape_properties.contains(name))
        {
            result = &_same_shape_constant_shape_properties;
        }
        else if (_same_shape_variable_shape_properties.contains(name))
        {
            result = &_same_shape_variable_shape_properties;
        }
        else if (_different_shape_properties.contains(name))
        {
            result = &_different_shape_properties;
        }
        else if (_different_shape_constant_shape_properties.contains(name))
        {
            result = &_different_shape_constant_shape_properties;
        }
        else if (_different_shape_variable_shape_properties.contains(name))
        {
            result = &_different_shape_variable_shape_properties;
        }

        if (result == nullptr)
        {
            throw std::runtime_error(std::format("Property named {} does not exist", name));
        }

        return *result;
    }


    void Properties::verify_property_does_not_exist(std::string const& name) const
    {
        if (contains(name))
        {
            throw std::runtime_error(std::format("Property named {} already exists", name));
        }
    }


    auto Properties::add(
        std::string const& name, hdf5::Datatype const& datatype, std::string const& description)
        -> same_shape::Property&
    {
        verify_property_does_not_exist(name);

        return _same_shape_properties.add(name, datatype, description);
    }


    auto Properties::add(
        std::string const& name,
        hdf5::Datatype const& datatype,
        hdf5::Shape const& shape,
        std::string const& description) -> same_shape::Property&
    {
        verify_property_does_not_exist(name);

        return _same_shape_properties.add(name, datatype, shape, description);
    }


    auto Properties::add(
        std::string const& name,
        hdf5::Datatype const& datatype,
        Rank const rank,
        std::string const& description) -> different_shape::Property&
    {
        verify_property_does_not_exist(name);

        return _different_shape_properties.add(name, datatype, rank, description);
    }


    auto create_properties(hdf5::Group& parent) -> Properties
    {
        if (hdf5::group_exists(parent, properties_tag))
        {
            throw std::runtime_error(
                std::format("Properties group already exists at {}", parent.id().pathname()));
        }

        hdf5::Group group{hdf5::create_group(parent, properties_tag)};

        same_shape::create_properties(group);
        same_shape::constant_shape::create_properties(group);
        same_shape::variable_shape::create_properties(group);
        different_shape::create_properties(group);
        different_shape::constant_shape::create_properties(group);
        different_shape::variable_shape::create_properties(group);

        return Properties{std::move(group)};
    }

}  // namespace lue::data_model

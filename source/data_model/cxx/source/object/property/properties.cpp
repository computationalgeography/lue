#include "lue/object/property/properties.hpp"
#include "lue/core/tag.hpp"
#include <boost/optional.hpp>
#include <fmt/core.h>
#include <functional>


namespace lue {
    namespace data_model {

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


        bool Properties::empty() const
        {
            return size() == 0;
        }


        std::size_t Properties::size() const
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


        bool Properties::contains(std::string const& name) const
        {
            return _same_shape_properties.contains(name) ||
                   _same_shape_constant_shape_properties.contains(name) ||
                   _same_shape_variable_shape_properties.contains(name) ||
                   _different_shape_properties.contains(name) ||
                   _different_shape_constant_shape_properties.contains(name) ||
                   _different_shape_variable_shape_properties.contains(name);
        }


        ShapePerObject Properties::shape_per_object(std::string const& name) const
        {
            boost::optional<ShapePerObject> result;

            if (_same_shape_properties.contains(name) ||
                _same_shape_constant_shape_properties.contains(name) ||
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
                throw std::runtime_error(fmt::format("Property named {} does not exist", name));
            }

            return *result;
        }


        ValueVariability Properties::value_variability(std::string const& name) const
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
                throw std::runtime_error(fmt::format("Property named {} does not exist", name));
            }

            return *result;
        }


        ShapeVariability Properties::shape_variability(std::string const& name) const
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
                    fmt::format("Property named {} does not exist or does not vary through time", name));
            }

            return *result;
        }


        /*!
            @brief      Return the collection group containing the property
                        named @a name
        */
        hdf5::Group& Properties::collection_group(std::string const& name)
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
                throw std::runtime_error(fmt::format("Property named {} does not exist", name));
            }

            return *result;
        }


        void Properties::verify_property_does_not_exist(std::string const& name) const
        {
            if (contains(name))
            {
                throw std::runtime_error(fmt::format("Property named {} already exists", name));
            }
        }


        same_shape::Property& Properties::add(
            std::string const& name, hdf5::Datatype const& datatype, std::string const& description)
        {
            verify_property_does_not_exist(name);

            return _same_shape_properties.add(name, datatype, description);
        }


        same_shape::Property& Properties::add(
            std::string const& name,
            hdf5::Datatype const& datatype,
            hdf5::Shape const& shape,
            std::string const& description)
        {
            verify_property_does_not_exist(name);

            return _same_shape_properties.add(name, datatype, shape, description);
        }


        different_shape::Property& Properties::add(
            std::string const& name,
            hdf5::Datatype const& datatype,
            Rank const rank,
            std::string const& description)
        {
            verify_property_does_not_exist(name);

            return _different_shape_properties.add(name, datatype, rank, description);
        }


        Properties create_properties(hdf5::Group& parent)
        {
            if (hdf5::group_exists(parent, properties_tag))
            {
                throw std::runtime_error(
                    fmt::format("Properties group already exists at {}", parent.id().pathname()));
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

    }  // namespace data_model
}  // namespace lue

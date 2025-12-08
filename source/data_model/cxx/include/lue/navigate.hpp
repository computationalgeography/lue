#pragma once
#include "lue/object/dataset.hpp"
#include <filesystem>
#include <optional>


namespace lue::data_model {

    // Try not to use this functionality!
    // These are all implementation details. Never depend on the knowledge
    // that is encoded in the implementation.

    template<typename Property>
    auto property_set_group(Property const& property) -> hdf5::Group
    {
        return property.parent().parent().parent();
    }


    template<typename Property>
    auto property_sets_group(Property const& property) -> hdf5::Group
    {
        return property_set_group(property).parent();
    }


    template<typename Property>
    auto phenomena_group(Property const& property) -> hdf5::Group
    {
        return property_sets_group(property).parent().parent();
    }


    template<typename Property>
    auto dataset_group(Property const& property) -> hdf5::Group
    {
        return phenomena_group(property).parent();
    }


    template<typename Property>
    auto property_set(Property const& property) -> PropertySet
    {
        return PropertySet{property_set_group(property)};
    }


    template<typename Property>
    auto property_sets(Property const& property) -> PropertySets
    {
        return PropertySets{Collection<PropertySet>{property_sets_group(property)}};
    }


    template<typename Property>
    auto dataset(Property& property) -> Dataset
    {
        return Dataset{hdf5::File{dataset_group(property)}};
    }


    /*!
        @brief      Given a @a property, find another property pointed to
                    by a @a path
    */
    template<typename Property>
    auto property(Property const& property, std::string const& pathname) -> PropertyGroup
    {
        // If path is absolute, it is relative to the dataset

        std::filesystem::path path{pathname};

        assert(path.is_absolute());

        // Get the dataset
        auto dataset_ = data_model::dataset(property);
        assert(dataset_.id().pathname() == "/");

        // Find the property
        std::optional<std::reference_wrapper<hdf5::Group>> parent;
        std::string property_name;

        auto it = path.begin();
        assert((*it).string() == "/");
        ++it;

        if (it != path.end())
        {
            auto const phenomenon_name = (*it).string();

            if (dataset_.phenomena().contains(phenomenon_name))
            {
                ++it;

                if (it != path.end())
                {
                    auto const property_set_name = (*it).string();

                    std::optional<std::reference_wrapper<PropertySets>> property_sets_opt;

                    {
                        auto& phenomenon = dataset_.phenomena()[phenomenon_name];

                        if (phenomenon.collection_property_sets().contains(property_set_name))
                        {
                            property_sets_opt = phenomenon.collection_property_sets();
                        }
                        else if (phenomenon.property_sets().contains(property_set_name))
                        {
                            property_sets_opt = phenomenon.property_sets();
                        }
                    }

                    if (property_sets_opt)
                    {
                        ++it;

                        if (it != path.end())
                        {
                            property_name = (*it).string();

                            {
                                PropertySets& property_sets_ = *property_sets_opt;
                                auto& property_set_ = property_sets_[property_set_name];

                                if (property_set_.properties().contains(property_name))
                                {

                                    // NOLINTNEXTLINE(bugprone-assert-side-effect)
                                    assert(++it == path.end());  // cppcheck-suppress assignmentInAssert

                                    parent = property_set_.properties().collection_group(property_name);
                                }
                            }
                        }
                    }
                }
            }
        }

        if (!parent)
        {
            throw std::runtime_error("Property cannot be found");
        }

        return PropertyGroup{*parent, property_name};
    }

}  // namespace lue::data_model

#pragma once
#include "lue/object/dataset.hpp"
#include <boost/filesystem.hpp>
#include <optional>


namespace lue {
namespace data_model {

// Try not to use this functionality!
// These are all implementation details. Never depend on the knowledge
// that is encoded in the implementation.

template<
    typename Property>
inline hdf5::Group property_set_group(
    Property& property)
{
    return property.parent().parent().parent();
}


template<
    typename Property>
inline hdf5::Group property_sets_group(
    Property& property)
{
    return property_set_group(property).parent();
}


template<
    typename Property>
inline hdf5::Group phenomena_group(
    Property& property)
{
    return property_sets_group(property).parent().parent();
}


template<
    typename Property>
inline hdf5::Group dataset_group(
    Property& property)
{
    return phenomena_group(property).parent();
}


template<
    typename Property>
inline PropertySet property_set(
    Property& property)
{
    return PropertySet{property_set_group(property)};
}


template<
    typename Property>
inline PropertySets property_sets(
    Property& property)
{
    return PropertySets{
        Collection<PropertySet>{property_sets_group(property)}};
}


template<
    typename Property>
inline Dataset dataset(
    Property& property)
{
    return Dataset{hdf5::File{dataset_group(property)}};
}


/*!
    @brief      Given a @a property, find another property pointed to
                by a @a path
*/
template<
    typename Property>
inline PropertyGroup property(
    Property& property,
    std::string const& pathname)
{
    // If path is absolute, it is relative to the dataset

    boost::filesystem::path path{pathname};

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

    if(it != path.end()) {
        auto const phenomenon_name = (*it).string();

        if(dataset_.phenomena().contains(phenomenon_name)) {

            ++it;

            if(it != path.end()) {

                auto const property_set_name = (*it).string();

                std::optional<std::reference_wrapper<PropertySets>> property_sets_opt;

                {
                    auto& phenomenon = dataset_.phenomena()[phenomenon_name];

                    if(phenomenon.collection_property_sets().contains(property_set_name)) {
                        property_sets_opt = phenomenon.collection_property_sets();
                    }
                    else if(phenomenon.property_sets().contains(property_set_name)) {
                        property_sets_opt = phenomenon.property_sets();
                    }
                }

                if(property_sets_opt) {
                    ++it;

                    if(it != path.end()) {
                        property_name = (*it).string();

                        {
                            PropertySets& property_sets_ = *property_sets_opt;
                            auto& property_set_ = property_sets_[property_set_name];

                            if(property_set_.properties().contains(property_name)) {

                                assert(++it == path.end());

                                parent =
                                    property_set_.properties().collection_group(
                                        property_name);
                            }
                        }
                    }
                }
            }
        }
    }

    if(!parent) {
        throw std::runtime_error("Property cannot be found");
    }

    return PropertyGroup{*parent, property_name};
}

}  // namespace data_model
}  // namespace lue

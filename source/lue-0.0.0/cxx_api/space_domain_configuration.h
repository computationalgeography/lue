#pragma once
#include "lue/cxx_api/define.h"
#include <string>


namespace lue {

std::string        space_domain_type_to_string(
                                        SpaceDomainType const type);

SpaceDomainType    parse_space_domain_type(
                                        std::string const& string);

std::string        space_domain_item_type_to_string(
                                        SpaceDomainItemType const type);

SpaceDomainItemType parse_space_domain_item_type(
                                        std::string const& string);

std::string        mobility_to_string  (Mobility const mobility);

Mobility           parse_mobility      (std::string const& string);


/*!
    @ingroup    lue_cxx_api_group
*/
class SpaceDomainConfiguration
{

public:

                   SpaceDomainConfiguration();

                   SpaceDomainConfiguration(
                                        SpaceDomainItemType const item_type);

                   SpaceDomainConfiguration(
                                        SpaceDomainType const type,
                                        SpaceDomainItemType const item_type);

                   SpaceDomainConfiguration(
                                        SpaceDomainType const type,
                                        SpaceDomainItemType const item_type,
                                        Mobility const mobility);

                   ~SpaceDomainConfiguration()=default;

    SpaceDomainType type               () const;

    SpaceDomainItemType item_type      () const;

    Mobility       mobility            () const;

private:

    SpaceDomainType _type;

    SpaceDomainItemType _item_type;

    Mobility       _mobility;

};

}  // namespace lue

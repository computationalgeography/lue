#pragma once
#include "lue/info/space.hpp"
#include "lue/core/configuration.hpp"
#include "lue/hdf5.hpp"


namespace lue {

/*!
    @brief      Aspects that determine how locations in space are represented
*/
using SpaceConfiguration = Configuration<
    Mobility,
    SpaceDomainItemType

>;


/*!
    @brief      Class representing the space domain
*/
class SpaceDomain:
    public hdf5::Group
{

public:

    explicit       SpaceDomain         (hdf5::Group& parent);

    explicit       SpaceDomain         (hdf5::Group&& group);

                   SpaceDomain         (SpaceDomain const&)=delete;

                   SpaceDomain         (SpaceDomain&&)=default;

                   ~SpaceDomain        ()=default;

    SpaceDomain&   operator=           (SpaceDomain const&)=delete;

    SpaceDomain&   operator=           (SpaceDomain&&)=default;

    SpaceConfiguration const&
                   configuration       () const;

    template<
        typename Value>
    Value          value               ();

private:

    SpaceConfiguration _configuration;

};


SpaceDomain        create_space_domain (hdf5::Group& parent,
                                        SpaceConfiguration const&
                                            configuration,
                                        hdf5::Datatype const& datatype,
                                        std::size_t const rank);

bool               space_domain_exists (hdf5::Group const& parent);


/*!
    @brief      Return instance representing the collection of space
                domain object arrays
    @tparam     Value Class corresponding with the instance's configuration()
*/
template<
    typename Value>
inline Value SpaceDomain::value()
{
    return Value{*this};
}

}  // namespace lue

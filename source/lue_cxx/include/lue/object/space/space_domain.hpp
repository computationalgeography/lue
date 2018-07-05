#pragma once
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
        typename T>
    T              value               ();

private:

    SpaceConfiguration _configuration;

};


SpaceDomain        create_space_domain (hdf5::Group& parent,
                                        SpaceConfiguration const&
                                            configuration,
                                        hdf5::Datatype const& datatype,
                                        std::size_t const rank);

bool               space_domain_exists (hdf5::Group const& parent);


template<
    typename T>
inline T SpaceDomain::value()
{
    return T{*this};
}

}  // namespace lue

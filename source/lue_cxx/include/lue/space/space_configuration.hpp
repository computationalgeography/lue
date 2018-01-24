#pragma once
#include "lue/aspect.hpp"


namespace lue {

class SpaceConfiguration:
    private Aspect<CollectionVariability>,
    private Aspect<Mobility>
{

public:

                   SpaceConfiguration  (CollectionVariability
                                            collection_variability,
                                        Mobility mobility);

                   SpaceConfiguration  (hdf5::Attributes const& attributes);

                   SpaceConfiguration  (SpaceConfiguration const&)=default;

                   SpaceConfiguration  (SpaceConfiguration&&)=default;

                   ~SpaceConfiguration ()=default;

    SpaceConfiguration& operator=      (SpaceConfiguration const&)=default;

    SpaceConfiguration& operator=      (SpaceConfiguration&&)=default;

    CollectionVariability
                   collection_variability() const;

    Mobility       mobility            () const;

    void           save                (hdf5::Attributes& attributes) const;

private:

    void           load                (hdf5::Attributes const& attributes);

};


inline SpaceConfiguration::SpaceConfiguration(
    CollectionVariability const collection_variability,
    Mobility const mobility)

    : Aspect<CollectionVariability>{collection_variability},
      Aspect<Mobility>{mobility}

{
}


inline SpaceConfiguration::SpaceConfiguration(
    hdf5::Attributes const& attributes)

    : Aspect<CollectionVariability>{attributes},
      Aspect<Mobility>{attributes}

{
}


inline void SpaceConfiguration::save(
    hdf5::Attributes& attributes) const
{
    Aspect<CollectionVariability>::save(attributes);
    Aspect<Mobility>::save(attributes);
}


inline void SpaceConfiguration::load(
    hdf5::Attributes const& attributes)
{
    Aspect<CollectionVariability>::load(attributes);
    Aspect<Mobility>::load(attributes);
}


inline CollectionVariability SpaceConfiguration::collection_variability() const
{
    return Aspect<CollectionVariability>::type();
}


inline Mobility SpaceConfiguration::mobility() const
{
    return Aspect<Mobility>::type();
}

}  // namespace lue

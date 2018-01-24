#pragma once
#include "lue/aspect.hpp"


namespace lue {

class ItemConfiguration:
    private Aspect<CollectionVariability>,
    private Aspect<ShapeVariability>,
    private Aspect<ShapePerItem>,
    private Aspect<ValueVariability>
{

public:

                   ItemConfiguration   (CollectionVariability
                                            collection_variability,
                                        ShapeVariability shape_variability,
                                        ShapePerItem shape_per_item,
                                        ValueVariability value_variability);

                   ItemConfiguration   (hdf5::Attributes const& attributes);

                   ItemConfiguration   (ItemConfiguration const&)=default;

                   ItemConfiguration   (ItemConfiguration&&)=default;

                   ~ItemConfiguration  ()=default;

    ItemConfiguration& operator=       (ItemConfiguration const&)=default;

    ItemConfiguration& operator=       (ItemConfiguration&&)=default;

    CollectionVariability
                   collection_variability() const;

    ShapeVariability
                   shape_variability   () const;

    ShapePerItem   shape_per_item      () const;

    ValueVariability
                   value_variability   () const;

    void           save                (hdf5::Attributes& attributes) const;

private:

    void           load                (hdf5::Attributes const& attributes);

};


inline ItemConfiguration::ItemConfiguration(
    CollectionVariability const collection_variability,
    ShapeVariability const shape_variability,
    ShapePerItem const shape_per_item,
    ValueVariability const value_variability)

    : Aspect<CollectionVariability>{collection_variability},
      Aspect<ShapeVariability>{shape_variability},
      Aspect<ShapePerItem>{shape_per_item},
      Aspect<ValueVariability>{value_variability}

{
}


inline ItemConfiguration::ItemConfiguration(
    hdf5::Attributes const& attributes)

    : Aspect<CollectionVariability>{attributes},
      Aspect<ShapeVariability>{attributes},
      Aspect<ShapePerItem>{attributes},
      Aspect<ValueVariability>{attributes}

{
}


inline void ItemConfiguration::save(
    hdf5::Attributes& attributes) const
{
    Aspect<CollectionVariability>::save(attributes);
    Aspect<ShapeVariability>::save(attributes);
    Aspect<ShapePerItem>::save(attributes);
    Aspect<ValueVariability>::save(attributes);
}


inline void ItemConfiguration::load(
    hdf5::Attributes const& attributes)
{
    Aspect<CollectionVariability>::load(attributes);
    Aspect<ShapeVariability>::load(attributes);
    Aspect<ShapePerItem>::load(attributes);
    Aspect<ValueVariability>::load(attributes);
}


inline CollectionVariability ItemConfiguration::collection_variability() const
{
    return Aspect<CollectionVariability>::type();
}


inline ShapeVariability ItemConfiguration::shape_variability() const
{
    return Aspect<ShapeVariability>::type();
}


inline ShapePerItem ItemConfiguration::shape_per_item() const
{
    return Aspect<ShapePerItem>::type();
}


inline ValueVariability ItemConfiguration::value_variability() const
{
    return Aspect<ValueVariability>::type();
}

}  // namespace lue

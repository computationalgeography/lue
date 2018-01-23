#pragma once
#include "lue/hdf5/attributes.hpp"


namespace lue {

/*!
    @brief      Does the collection of items change through time
*/
enum class CollectionVariability
{

    //! Collection of items doesn't change through time
    constant

};


/*!
    @brief      Does the shape of property values change through time
*/
enum class ShapeVariability
{

    //! Shape of property values doesn't change through time
    constant

};


/*!
    @brief      The shape of the value of different items can be the
                same or different
*/
enum class ShapePerItem
{

    //! The value of all items have the same shape
    same,

    //! The value of all items have a different shape (potentially)
    different

};


/*!
    @brief      Does the value of property values change through time
*/
enum class ValueVariability
{

    //! Item values don't change through time
    constant,

    //! Item values change through time
    variable

};


class Configuration
{

public:

                   Configuration       (CollectionVariability
                                            collection_variability,
                                        ShapeVariability shape_variability,
                                        ShapePerItem shape_per_item,
                                        ValueVariability value_variability);

                   Configuration       (hdf5::Attributes const& attributes);

                   Configuration       (Configuration const&)=default;

                   Configuration       (Configuration&&)=default;

                   ~Configuration      ()=default;

    Configuration& operator=           (Configuration const&)=default;

    Configuration& operator=           (Configuration&&)=default;

    CollectionVariability
                   collection_variability () const;

    ShapeVariability
                   shape_variability   () const;

    ShapePerItem   shape_per_item      () const;

    ValueVariability
                   value_variability   () const;

    void           save                (hdf5::Attributes& attributes) const;

private:

    CollectionVariability _collection_variability;

    ShapeVariability _shape_variability;

    ShapePerItem   _shape_per_item;

    ValueVariability _value_variability;

    void           load                (hdf5::Attributes const& attributes);

};

}  // namespace lue

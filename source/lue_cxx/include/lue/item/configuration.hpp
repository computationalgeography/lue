#pragma once
#include "lue/hdf5/attributes.hpp"


namespace lue {

class Configuration
{

public:

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


                   Configuration       (CollectionVariability const
                                            collection_variability,
                                        ShapeVariability const
                                            shape_variability);

                   Configuration       (hdf5::Attributes const& attributes);

                   Configuration       (Configuration const&)=default;

                   Configuration       (Configuration&&)=default;

                   ~Configuration      ()=default;

    Configuration& operator=           (Configuration const&)=default;

    Configuration& operator=           (Configuration&&)=default;

    CollectionVariability
                   collection_variability () const;

    ShapeVariability
                   shape_variability () const;

    void           save                (hdf5::Attributes& attributes) const;

private:

    CollectionVariability _collection_variability;

    ShapeVariability _shape_variability;

    void           load                (hdf5::Attributes const& attributes);

};

}  // namespace lue

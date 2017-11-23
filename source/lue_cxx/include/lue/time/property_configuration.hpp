#pragma once
#include "lue/hdf5/attributes.hpp"


namespace lue {
namespace time {

class PropertyConfiguration
{

public:

    /*!
        @brief      Does the shape of property values change through time
    */
    enum class ShapeVariability
    {

        //! Shape of property values doesn't change through time
        constant

    };


                   PropertyConfiguration(
                                        ShapeVariability const
                                            shape_variability);

                   PropertyConfiguration(
                                        hdf5::Attributes const& attributes);

                   PropertyConfiguration(
                                        PropertyConfiguration const&)=default;

                   ~PropertyConfiguration()=default;

    PropertyConfiguration&
                   operator=           (PropertyConfiguration const&)=default;

    ShapeVariability
                   shape_variability   () const;

    void           save                (hdf5::Attributes& attributes) const;

private:

    ShapeVariability _shape_variability;

    void           load                (hdf5::Attributes const& attributes);

};

}  // namespace time
}  // namespace lue

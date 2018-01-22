#pragma once
#include "lue/constant_size/time/located/property_set.hpp"
#include "lue/constant_size/time/located/constant_shape/different_shape/property.hpp"
#include "lue/constant_size/time/omnipresent/same_shape/property.hpp"
#include "lue/hl/raster_stack_discretization.hpp"
#include "lue/hl/raster_stack_domain.hpp"
#include "lue/dataset.hpp"


namespace lue {
namespace hl {

namespace omnipresent = constant_size::time::omnipresent;
namespace located = constant_size::time::located;


class RasterStack
{

public:

    class Band
    {

    public:

                   Band                (located::constant_shape::different_shape
                                            ::Property&& property);

        hdf5::Datatype file_datatype   () const;

        hdf5::Datatype memory_datatype () const;

        void       read                (void* buffer) const;

        // void       read                (hdf5::Hyperslab const& hyperslab,
        //                                 void* buffer) const;

        void       write               (void const* buffer);

        void       write               (hdf5::Dataspace const& memory_dataspace,
                                        hdf5::Hyperslab const& hyperslab,
                                        void const* buffer);

    private:

        located::constant_shape::different_shape::Property _property;

    };


                   RasterStack         (hdf5::Identifier const& phenomenon_id,
                                        std::string const& property_set_name);
                                        // hdf5::Identifier const&
                                        //     time_discretization_property_id,
                                        // hdf5::Identifier const&
                                        //     space_discretization_property_id);

                   // RasterStack         (hdf5::Identifier const& phenomenon_id,
                   //                      std::string const& property_set_name);

                   RasterStack         (RasterStack const&)=delete;

                   RasterStack         (RasterStack&&)=default;

                   ~RasterStack        ()=default;

    RasterStack&   operator=           (RasterStack const& other)=delete;

    RasterStack&   operator=           (RasterStack&& other)=default;

    RasterStackDomain const&
                   domain              () const;

    RasterStackDiscretization const&
                   discretization      () const;

    Band           add_band            (std::string const& name,
                                        hdf5::Datatype const& datatype);

    Band           band                (std::string const& name) const;

private:

    void           read                ();

    Phenomenon     _phenomenon;

    located::PropertySet _property_set;

    omnipresent::same_shape::Property _time_discretization_property;

    omnipresent::same_shape::Property _space_discretization_property;

    RasterStackDomain _domain;

    RasterStackDiscretization _discretization;

};


bool               raster_stack_exists (Dataset& dataset,
                                        std::string const& phenomenon_name,
                                        std::string const& property_set_name);

RasterStack        create_raster_stack (Dataset& dataset,
                                        std::string const& phenomenon_name,
                                        std::string const& property_set_name,
                                        RasterStackDomain const& domain,
                                        RasterStackDiscretization const&
                                            discretization);

}  // namespace hl
}  // namespace lue

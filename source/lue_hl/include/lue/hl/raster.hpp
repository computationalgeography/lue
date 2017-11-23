#pragma once
#include "lue/hl/raster_discretization.hpp"
#include "lue/hl/raster_domain.hpp"
#include "lue/constant_size/time/omnipresent/different_shape/property.hpp"
#include "lue/constant_size/time/omnipresent/same_shape/property.hpp"
#include "lue/constant_size/time/omnipresent/property_set.hpp"
#include "lue/dataset.hpp"


namespace lue {
namespace hl {

namespace omnipresent = constant_size::time::omnipresent;


class Raster
{

public:

    class Band
    {

    public:

                   Band                (omnipresent::different_shape::
                                            Property&& property);

        hdf5::Datatype file_datatype   () const;

        hdf5::Datatype memory_datatype () const;

        void       read                (void* buffer) const;

        void       read                (hdf5::Hyperslab const& hyperslab,
                                        void* buffer) const;

        void       write               (void const* buffer);

        void       write               (hdf5::Dataspace const& memory_dataspace,
                                        hdf5::Hyperslab const& hyperslab,
                                        void const* buffer);

    private:

        omnipresent::different_shape::Property _property;

    };

                   Raster              (hdf5::Identifier const& id);

                   Raster              (Dataset& dataset,
                                        std::string const& phenomenon_name,
                                        std::string const& property_set_name);

                   Raster              (Raster const& other)=delete;

                   Raster              (Raster&& other)=default;

                   ~Raster             ()=default;

    Raster&        operator=           (Raster const& other)=delete;

    Raster&        operator=           (Raster&& other)=default;

    RasterDomain const& domain         () const;

    RasterDiscretization const& discretization() const;

    Band           add_band            (std::string const& name,
                                        hdf5::Datatype const& datatype);

    Band           band                (std::string const& name) const;

private:

    void           read                ();

    void           read_discretization ();

    void           read_domain         ();

    omnipresent::PropertySet _property_set;

    omnipresent::same_shape::Property _discretization_property;

    RasterDiscretization _discretization;

    RasterDomain   _domain;

};


// bool               raster_exists       (Dataset const& dataset,
//                                         std::string const& phenomenon_name,
//                                         std::string const& property_set_name);

Raster             create_raster       (Dataset& dataset,
                                        std::string const& phenomenon_name,
                                        std::string const& property_set_name,
                                        RasterDomain const& domain,
                                        RasterDiscretization const&
                                            discretization);

}  // namespace hl
}  // namespace lue

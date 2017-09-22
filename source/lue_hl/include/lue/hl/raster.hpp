#pragma once
#include "lue/constant_size/time/omnipresent/different_shape/property.hpp"
#include "lue/constant_size/time/omnipresent/same_shape/property.hpp"
#include "lue/constant_size/time/omnipresent/property_set.hpp"
#include "lue/dataset.hpp"
#include <array>


namespace lue {
namespace hl {

namespace omnipresent = constant_size::time::omnipresent;


class Raster
{

public:

    class Discretization
    {

    public:

                   Discretization      ();

                   Discretization      (hsize_t const nr_rows,
                                        hsize_t const nr_cols);

        hsize_t const* shape           () const;

        hsize_t*   shape               ();

        hsize_t    nr_rows             () const;

        hsize_t    nr_cols             () const;

    private:

        hsize_t    _shape[2];

    };


    class Domain
    {

    public:

                   Domain              ();

                   Domain              (std::string const& crs,
                                        double const west,
                                        double const south,
                                        double const east,
                                        double const north);

        std::string const&
                   crs                 () const;

        double     west                () const;

        double     south               () const;

        double     east                () const;

        double     north               () const;

        std::array<double, 4>&
                   coordinates         ();

        std::array<double, 4> const&
                   coordinates         () const;

    private:

        std::string const _crs;

        std::array<double, 4> _coordinates;

    };


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

    Domain const&  domain              () const;

    Discretization const& discretization() const;

    Band           add_band            (std::string const& name,
                                        hdf5::Datatype const& datatype);

    Band           band                (std::string const& name) const;

private:

    omnipresent::PropertySet _property_set;

    omnipresent::same_shape::Property _discretization_property;

    Discretization _discretization;

    Domain         _domain;

};


// bool               raster_exists       (Dataset const& dataset,
//                                         std::string const& phenomenon_name,
//                                         std::string const& property_set_name);

Raster             create_raster       (Dataset& dataset,
                                        std::string const& phenomenon_name,
                                        std::string const& property_set_name,
                                        Raster::Domain const& domain,
                                        Raster::Discretization const&
                                            discretization);

}  // namespace hl
}  // namespace lue

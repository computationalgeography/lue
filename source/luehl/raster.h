#pragma once
#include "lue/constant_size/time/omnipresent/same_shape/property.h"
#include "lue/constant_size/time/omnipresent/property_set.h"
#include "lue/dataset.h"


namespace lue {
namespace hl {

namespace omnipresent = constant_size::time::omnipresent;


class Raster
{

public:

    class Discretization
    {

    public:

                   Discretization      (size_t const nr_rows,
                                        size_t const nr_cols);

        size_t const* shape            () const;

        size_t     nr_rows             () const;

        size_t     nr_cols             () const;

    private:

        size_t const _shape[2];

    };


    class Domain
    {

    public:

                   Domain              (std::string const& crs,
                                        double const west,
                                        double const north,
                                        double const east,
                                        double const south);

    double const*  coordinates         () const;

    private:

        std::string const _crs;

        double const _coordinates[4];

    };

                   Raster              (Dataset&& dataset,
                                        omnipresent::PropertySet&& property_set,
                                        omnipresent::same_shape::Property&&
                                            discretization);

                   Raster              (Raster const& other)=delete;

                   Raster              (Raster&& other)=default;

                   ~Raster             ()=default;

    Raster&        operator=           (Raster const& other)=delete;

    Raster&        operator=           (Raster&& other)=default;

    // void           write_band          (std::string const& name,
    //                                     hdf5::Datatype const& datatype,
    //                                     void const* buffer);

private:

    Dataset        _dataset;

    omnipresent::PropertySet _property_set;

    omnipresent::same_shape::Property _discretization;

};


Raster             create_raster       (std::string const& dataset_name,
                                        std::string const& phenomenon_name,
                                        std::string const& property_set_name,
                                        Raster::Domain const& domain,
                                        Raster::Discretization const&
                                            discretization);

}  // namespace hl
}  // namespace lue

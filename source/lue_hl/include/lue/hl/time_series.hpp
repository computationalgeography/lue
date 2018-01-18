#pragma once
#include "lue/hl/time_series_discretization.hpp"
#include "lue/hl/time_series_domain.hpp"
#include "lue/constant_size/time/located/shared/constant_shape/same_shape/property.hpp"
#include "lue/dataset.hpp"
#include <vector>


namespace lue {
namespace hl {

namespace located = constant_size::time::located;


class TimeSeries
{

public:

    class SpaceDomain
    {

    public:

        using Coordinate = double;
        using Coordinates = std::vector<Coordinate>;

                   SpaceDomain         ();

                   SpaceDomain         (std::string const& crs,
                                        Coordinates&& coordinates);

        std::string const&
                   crs                 () const;

        Coordinates const&
                   coordinates         () const;

        std::size_t nr_points          () const;

    private:

        std::string _crs;

        Coordinates _coordinates;

    };


                   TimeSeries          (hdf5::Identifier const& phenomenon_id,
                                        std::string const& property_set_name,
                                        std::string const& property_name);

                   TimeSeries          (TimeSeries const& other)=delete;

                   TimeSeries          (TimeSeries&& other)=default;

                   ~TimeSeries         ()=default;

    TimeSeries&    operator=           (TimeSeries const& other)=delete;

    TimeSeries&    operator=           (TimeSeries&& other)=default;

    std::string    name                () const;

    hsize_t        nr_items            () const;

    TimeSeriesDomain const& time_domain() const;

    TimeSeriesDiscretization const& time_discretization() const;

    SpaceDomain const& space_domain    () const;

    void           write               (std::size_t item_idx,
                                        double const* values);

    void           read                (std::size_t item_idx,
                                        double* values);

private:

    hdf5::Hyperslab hyperslab          (std::size_t const item_idx) const;

    Phenomenon     _phenomenon;

    located::PropertySet _property_set;

    located::shared::constant_shape::same_shape::Property _value_property;

    TimeSeriesDomain _time_domain;

    SpaceDomain    _space_domain;

    TimeSeriesDiscretization _time_discretization;

};


TimeSeries         create_time_series  (Dataset& dataset,
                                        std::string const& phenomenon_name,
                                        std::string const& property_set_name,
                                        TimeSeriesDomain const& time_domain,
                                        TimeSeries::SpaceDomain const&
                                            space_domain,
                                        std::string const& property_name,
                                        TimeSeriesDiscretization const&
                                            time_discretization);

}  // namespace hl
}  // namespace lue

#pragma once
#include "lue/clock.hpp"
#include "lue/constant_size/time/located/shared/property_set.hpp"
#include "lue/constant_size/time/located/shared/constant_shape/same_shape/property.hpp"
#include "lue/dataset.hpp"
#include <array>
#include <vector>


namespace lue {
namespace hl {

namespace shared = constant_size::time::located::shared;


class TimeSeries
{

public:

    class TimeDomain
    {

    public:

        using Coordinates = std::array<time::DurationCount, 2>;

                   TimeDomain          ();

                   TimeDomain          (Clock const& clock,
                                        time::DurationCount const start,
                                        time::DurationCount const end);

        Clock const&
                   clock               () const;

        Coordinates const&
                   coordinates         () const;

    private:

        Clock      _clock;

        Coordinates _coordinates;

    };


    class TimeDiscretization
    {

    public:

                   TimeDiscretization  ();

                   TimeDiscretization  (hsize_t const nr_steps);

        hsize_t const* shape           () const;

        hsize_t    nr_steps            () const;

    private:

        hsize_t    _shape[1];

    };


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

    TimeDomain const& time_domain      () const;

    TimeDiscretization const& time_discretization() const;

    SpaceDomain const& space_domain    () const;

    void           write               (std::size_t item_idx,
                                        double const* values);

    void           read                (std::size_t item_idx,
                                        double* values);

private:

    hdf5::Hyperslab hyperslab          (std::size_t const item_idx) const;

    Phenomenon     _phenomenon;

    shared::PropertySet _property_set;

    shared::constant_shape::same_shape::Property _value_property;

    TimeDomain     _time_domain;

    SpaceDomain    _space_domain;

    TimeDiscretization _time_discretization;

};


TimeSeries         create_time_series  (Dataset& dataset,
                                        std::string const& phenomenon_name,
                                        std::string const& property_set_name,
                                        TimeSeries::TimeDomain const&
                                            time_domain,
                                        TimeSeries::SpaceDomain const&
                                            space_domain,
                                        std::string const& property_name,
                                        TimeSeries::TimeDiscretization const&
                                            time_discretization);

}  // namespace hl
}  // namespace lue

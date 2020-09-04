#pragma once
#include "wildfire_model_base.hpp"
#include "lue/data_model/hl/raster_view.hpp"


namespace lue {

class WildfireModel final:
    public WildfireModelBase
{

public:

                   WildfireModel       (data_model::Count nr_time_steps,
                                        RasterShape const& raster_shape,
                                        RasterShape const& partition_shape);

                   WildfireModel       (WildfireModel const&)=default;

                   WildfireModel       (WildfireModel&&)=default;

                   ~WildfireModel      ();

    WildfireModel& operator=           (WildfireModel const&)=default;

    WildfireModel& operator=           (WildfireModel&&)=default;

    void           initialize          () final;

    void           simulate            (Count time_step) final;

    void           terminate           ();

private:

    using ConstantRasterView =
        data_model::constant::RasterView<std::shared_ptr<data_model::Dataset>>;
    using VariableRasterView =
        data_model::variable::RasterView<std::shared_ptr<data_model::Dataset>>;

    BooleanRaster  initial_fire        () const;

    data_model::Count _nr_time_steps;

    RasterShape    _raster_shape;

    RasterShape    _partition_shape;

    BooleanRaster  _clone;

    bool           _with_io;

    std::shared_ptr<data_model::Dataset> _dataset_ptr;

    ConstantRasterView _constant_raster_view;

    VariableRasterView _variable_raster_view;

    VariableRasterView::Layer _fire_layer;

    VariableRasterView::Layer _burning_layer;

    VariableRasterView::Layer _fire_age_layer;

    VariableRasterView::Layer _state_layer;

    std::vector<hpx::future<void>> _written;

};

}  // namespace lue

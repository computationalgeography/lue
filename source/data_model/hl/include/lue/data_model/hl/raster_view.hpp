#pragma once
#include "lue/data_model/hl/dataset_view.hpp"


namespace lue {

/*!
    @brief      Class for performing I/O of variable rasters of a
                single area

    The use-case catered for by this class is of conventional
    spatio-temporal model data, as output by a PCRaster model. There is
    a single time box and space box that will be filled by the model.
*/
class RasterView:
    public DatasetView
{

public:

    using TimeBox = std::array<ElementT<lue::TimeBox>, 2>;

    using SpaceBox = std::array<double, 4>;

    using Layer = same_shape::constant_shape::Value;

                   RasterView          (DatasetPtr dataset,
                                        std::string const& phenomenon_name,
                                        std::string const& property_set_name);

                   RasterView          (RasterView const&)=default;

                   RasterView          (RasterView&&)=default;

                   ~RasterView         ()=default;

    RasterView&    operator=           (RasterView const&)=default;
 
    RasterView&    operator=           (RasterView&&)=default;
 
    std::string const& phenomenon_name () const;
 
    std::string const& property_set_name() const;

    TimeBox const& time_box            () const;

    SpaceBox const& space_box          () const;
 
    hdf5::Shape::value_type
                   nr_time_steps       () const;

    hdf5::Shape const& raster_shape    () const;

    Count          nr_layers           () const;

    std::vector<std::string>
                   layer_names         () const;

    bool           contains            (std::string const& name);

    template<
        typename Element>
    Layer          add_layer           (std::string const& name);

    Layer          add_layer           (std::string const& name,
                                        hdf5::Datatype const& datatype);

private:

    std::string    _phenomenon_name;
 
    std::string    _property_set_name;

    ID             _id;

    TimeBox        _time_box;

    hdf5::Shape    _time_grid;
 
    SpaceBox       _space_box;

    hdf5::Shape    _space_grid;

    std::vector<std::string> _layer_names;

};


template<
    typename Element>
RasterView::Layer RasterView::add_layer(
    std::string const& name)
{
    return add_layer(
        name, hdf5::Datatype{hdf5::NativeDatatypeTraits<Element>::type_id()});
}


RasterView         create_raster_view  (DatasetPtr dataset,
                                        std::string const& phenomenon_name,
                                        std::string const& property_set_name,
                                        Clock const& clock,
                                        Count nr_time_steps,
                                        RasterView::TimeBox const& time_box,
                                        hdf5::Shape const& raster_shape,
                                        RasterView::SpaceBox const& space_box);

}  // namespace lue

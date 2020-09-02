#include "wildfire_model.hpp"
#include "lue/framework/algorithm/array_partition_id.hpp"
#include "lue/framework/algorithm/less_than.hpp"
#include "lue/framework/algorithm/locality_id.hpp"
#include "lue/framework/algorithm/uniform.hpp"
#include "lue/framework/io.hpp"


namespace ldm = lue::data_model;
namespace lh5 = lue::hdf5;


namespace lue {
namespace {

static std::string const dataset_pathname{"burn_wilderness.lue"};
static std::string const phenomenon_name{"area"};
static std::string const constant_property_set_name{"constant"};
static std::string const variable_property_set_name{"variable"};
static ldm::Clock const clock{ldm::time::Unit::day, 1};

lh5::Shape shape_to_shape(
        WildfireModel::RasterShape const& input_shape)
{
    // In non-Debug builds, lh5::Shape(raster_shape.begin(),
    // raster_shape.end()) results in an undefined symbol error (pointing
    // to std::allocator<unsigned long long>::allocator()). Therefore,
    // this hack.
    lh5::Shape output_shape(2);
    std::copy(input_shape.begin(), input_shape.end(), output_shape.begin());

    return output_shape;
}

}  // Anonymous namespace


WildfireModel::WildfireModel(
    ldm::Count const nr_time_steps,
    RasterShape const& raster_shape,
    RasterShape const& partition_shape):

    WildfireModelBase{},
    _nr_time_steps{nr_time_steps},
    _raster_shape{raster_shape},
    _partition_shape{partition_shape},
    _clone{_raster_shape, _partition_shape},
    _dataset_ptr{
        std::make_shared<ldm::Dataset>(ldm::create_dataset(dataset_pathname))},
    _constant_raster_view{
        ldm::constant::create_raster_view(
            _dataset_ptr,
            phenomenon_name, constant_property_set_name,
            shape_to_shape(raster_shape),
            {0.0, 0.0, double(raster_shape[1]), double(raster_shape[0])})},
    _variable_raster_view{
        ldm::variable::create_raster_view(
            _dataset_ptr,
            phenomenon_name, variable_property_set_name,
            // Also store initial state
            clock, _nr_time_steps + 1, {0, _nr_time_steps + 1},
            shape_to_shape(raster_shape),
            {0.0, 0.0, double(raster_shape[1]), double(raster_shape[0])})},
    _fire_layer{_variable_raster_view.add_layer<BooleanElement>("fire")},
    _burning_layer{_variable_raster_view.add_layer<BooleanElement>("burning")},
    _fire_age_layer{_variable_raster_view.add_layer<CountElement>("fire_age")},
    _state_layer{_variable_raster_view.add_layer<NominalElement>("state")}

{
}


WildfireModel::~WildfireModel()
{
    hpx::wait_all_n(_written.begin(), _written.size());
}


WildfireModel::BooleanRaster WildfireModel::initial_fire() const
{
    return uniform(_clone, 0.0, 1.0) < 1e-5;
}


void WildfireModel::initialize()
{
    WildfireModelBase::initialize();

    PartitionedArray<std::uint32_t, 2> locality_id{lue::locality_id(_clone)};
    PartitionedArray<std::uint64_t, 2> array_partition_id{lue::array_partition_id(_clone)};

    ConstantRasterView::Layer locality_id_layer{
        _constant_raster_view.add_layer<std::uint32_t>("locality_id")};
    ConstantRasterView::Layer array_partition_id_layer{
        _constant_raster_view.add_layer<std::uint64_t>("array_partition_id")};
    ConstantRasterView::Layer ignite_probability_layer{
        _constant_raster_view.add_layer<ScalarElement>("ignite_probability")};
    ConstantRasterView::Layer spot_ignite_probability_layer{
        _constant_raster_view.add_layer<ScalarElement>("spot_ignite_probability")};

    _written.push_back(write(locality_id, locality_id_layer));
    _written.push_back(write(array_partition_id, array_partition_id_layer));
    _written.push_back(write(ignite_probability(), ignite_probability_layer));
    _written.push_back(write(spot_ignite_probability(), spot_ignite_probability_layer));
}


void WildfireModel::simulate(
    Count const time_step)
{
    WildfireModelBase::simulate(time_step);

    _written.push_back(write(fire(), _fire_layer, time_step));
    _written.push_back(write(burning(), _burning_layer, time_step));
    _written.push_back(write(fire_age(), _fire_age_layer, time_step));
    _written.push_back(write(state(), _state_layer, time_step));
}

}  // namespace lue

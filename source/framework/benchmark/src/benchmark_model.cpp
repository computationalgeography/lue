#include "lue/framework/benchmark/benchmark_model.hpp"
#include "lue/assert.hpp"
// #include "lue/framework/algorithm/array_like.hpp"
// #include "lue/framework/algorithm/array_partition_id.hpp"
// #include "lue/framework/algorithm/locality_id.hpp"
// #include "lue/framework/algorithm/zonal_sum.hpp"
// #include "lue/framework/io.hpp"
// #include "lue/data_model/hl/raster_view.hpp"
#include <hpx/include/iostreams.hpp>
#include <algorithm>


namespace lue {
namespace benchmark {

template<
    typename Element,
    Rank rank>
BenchmarkModel<Element, rank>::BenchmarkModel(
    Task const& task,
    std::size_t const max_tree_depth):

    Model{},
    _count{0},
    _max_tree_depth{max_tree_depth},
    _semaphore{static_cast<std::int64_t>(_max_tree_depth)},
    _state{},
    _array_shape{},
    _partition_shape{}

{
    std::copy(
        task.array_shape().begin(), task.array_shape().end(),
        _array_shape.begin());

    std::copy(
        task.partition_shape().begin(), task.partition_shape().end(),
        _partition_shape.begin());
}


template<
    typename Element,
    Rank rank>
typename BenchmarkModel<Element, rank>::Shape const&
    BenchmarkModel<Element, rank>::array_shape() const
{
    return _array_shape;
}


template<
    typename Element,
    Rank rank>
typename BenchmarkModel<Element, rank>::Shape const&
    BenchmarkModel<Element, rank>::partition_shape() const
{
    return _partition_shape;
}


template<
    typename Element,
    Rank rank>
void BenchmarkModel<Element, rank>::set_result(
    Result const& result)
{
    lue_assert(result.shape_in_partitions().size() == rank);

    _result = result;
}


template<
    typename Element,
    Rank rank>
typename BenchmarkModel<Element, rank>::Result const&
    BenchmarkModel<Element, rank>::result() const
{
    lue_assert(_result.shape_in_partitions().size() == rank);

    return _result;
}


template<
    typename Element,
    Rank rank>
void BenchmarkModel<Element, rank>::preprocess()
{
    _state = Array{this->array_shape(), this->partition_shape()};

    do_preprocess();

    // Time spent calculating state at t0 is not part of the
    // benchmark. Compare this to reading form a dataset. Leave I/O out.
    hpx::wait_all_n(_state.partitions().begin(), _state.nr_partitions());

    if(_count == 0) {
        hpx::cout << describe(_state) << hpx::endl;
    }

    // // Optionally, write state -------------------------------------------------
    // bool const write_info{false};

    // if(write_info)
    // {
    //     namespace ldm = lue::data_model;
    //     namespace lh5 = lue::hdf5;

    //     // using Shape = typename BenchmarkModel<rank>::Shape;
    //     using ConstantRasterView =
    //         ldm::constant::RasterView<std::shared_ptr<ldm::Dataset>>;

    //     std::string const dataset_pathname{"focal_mean.lue"};
    //     auto dataset_ptr{std::make_shared<ldm::Dataset>(
    //         ldm::create_dataset(dataset_pathname))};

    //     // Shape const array_shape{this->array_shape()};
    //     lh5::Shape array_shape(rank);
    //     std::copy(this->array_shape().begin(), this->array_shape().end(), array_shape.begin());

    //     std::string const phenomenon_name{"area"};
    //     std::string const constant_property_set_name{"constant"};

    //     ConstantRasterView constant_raster_view{
    //         ldm::constant::create_raster_view(
    //             dataset_ptr,
    //             phenomenon_name, constant_property_set_name,
    //             array_shape,
    //             {0.0, 0.0, double(array_shape[1]), double(array_shape[0])})};

    //     PartitionedArray<std::uint32_t, 2> locality_id{
    //         lue::locality_id(_state)};
    //     PartitionedArray<std::uint64_t, 2> array_partition_id{
    //         lue::array_partition_id(_state)};
    //     PartitionedArray<std::uint64_t, 2> nr_elements_per_locality{
    //         zonal_sum(array_like<std::uint64_t>(_state, 1), locality_id)};

    //     ConstantRasterView::Layer locality_id_layer{
    //         constant_raster_view.add_layer<std::uint32_t>(
    //             "locality_id")};
    //     ConstantRasterView::Layer array_partition_id_layer{
    //         constant_raster_view.add_layer<std::uint32_t>(
    //             "array_partition_id")};
    //     ConstantRasterView::Layer nr_elements_per_locality_layer{
    //         constant_raster_view.add_layer<std::uint32_t>(
    //             "nr_elements_per_locality")};

    //     std::vector<hpx::future<void>> written;
    //     written.push_back(
    //         write(locality_id, locality_id_layer));
    //     written.push_back(
    //         write(array_partition_id, array_partition_id_layer));
    //     written.push_back(
    //         write(nr_elements_per_locality, nr_elements_per_locality_layer));
    //     hpx::wait_all_n(written.begin(), written.size());
    // }
}


template<
    typename Element,
    Rank rank>
void BenchmarkModel<Element, rank>::initialize()
{
    hpx::cout << '[' << hpx::flush;

    do_initialize();
}


template<
    typename Element,
    Rank rank>
void BenchmarkModel<Element, rank>::simulate(
    Count const time_step)
{
    do_simulate(time_step);

    hpx::cout << '.' << hpx::flush;

    // every nd time steps, attach additional continuation which will
    // trigger the semaphore once computation has reached this point
    if((time_step % _max_tree_depth) == 0)
    {
        _state.partitions()[0].then(

            [this, time_step](auto const&)
            {
                // inform semaphore about new lower limit
                _semaphore.signal(time_step);
            });
    }

    // suspend if the tree has become too deep, the continuation above
    // will resume this thread once the computation has caught up
    _semaphore.wait(time_step);
}


template<
    typename Element,
    Rank rank>
void BenchmarkModel<Element, rank>::terminate()
{
    do_terminate();

    hpx::wait_all_n(_state.partitions().begin(), _state.nr_partitions());

    hpx::cout << "]\n" << hpx::flush;
}


template<
    typename Element,
    Rank rank>
void BenchmarkModel<Element, rank>::postprocess()
{
    do_postprocess();

    this->set_result(AlgorithmBenchmarkResult{_state.partitions().shape()});

    ++_count;
}


template<
    typename Element,
    Rank rank>
void BenchmarkModel<Element, rank>::do_preprocess()
{
}


template<
    typename Element,
    Rank rank>
void BenchmarkModel<Element, rank>::do_initialize()
{
}


template<
    typename Element,
    Rank rank>
void BenchmarkModel<Element, rank>::do_simulate(
    Count const /* time_step */)
{
}


template<
    typename Element,
    Rank rank>
void BenchmarkModel<Element, rank>::do_terminate()
{
}


template<
    typename Element,
    Rank rank>
void BenchmarkModel<Element, rank>::do_postprocess()
{
}


template class BenchmarkModel<std::int32_t, 2>;
template class BenchmarkModel<double, 2>;

}  // namespace benchmark
}  // namespace lue

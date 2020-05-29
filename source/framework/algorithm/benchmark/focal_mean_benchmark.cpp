#include "benchmark_model.hpp"
#include "lue/framework/algorithm/array_like.hpp"
#include "lue/framework/algorithm/array_partition_id.hpp"
#include "lue/framework/algorithm/focal_mean.hpp"
#include "lue/framework/algorithm/kernel.hpp"
#include "lue/framework/algorithm/locality_id.hpp"
#include "lue/framework/algorithm/zonal_sum.hpp"
#include "lue/framework/algorithm/uniform.hpp"
#include "lue/framework/algorithm/serialize/kernel.hpp"
#include "lue/framework/benchmark/hpx_main.hpp"
#include "lue/framework/benchmark/model_benchmark.hpp"
#include "lue/framework/io.hpp"
#include "lue/data_model/hl/raster_view.hpp"


namespace lue {
namespace benchmark {

template<
    typename Element,
    Rank rank>
class FocalMeanBenchmarkModel final:
    public BenchmarkModel<rank>
{

public:

    using Array = PartitionedArray<Element, rank>;

    using Kernel = lue::Kernel<bool, rank>;

                   FocalMeanBenchmarkModel(
                                        Task const& task);

                   FocalMeanBenchmarkModel(
                                        FocalMeanBenchmarkModel const&)=default;

                   FocalMeanBenchmarkModel(
                                        FocalMeanBenchmarkModel&&)=default;

                   ~FocalMeanBenchmarkModel()=default;

    FocalMeanBenchmarkModel&
                   operator=           (FocalMeanBenchmarkModel const&)=default;

    FocalMeanBenchmarkModel&
                   operator=           (FocalMeanBenchmarkModel&&)=default;

    void           preprocess          ();

    void           initialize          ();

    void           simulate            (Count time_step);

    void           terminate           ();

    void           postprocess         ();

private:

    Array _state;

    Kernel _kernel;

};


template<
    typename Element,
    std::size_t rank>
FocalMeanBenchmarkModel<Element, rank>::FocalMeanBenchmarkModel(
    Task const& task):

    BenchmarkModel<rank>{task},
    _state{},
    _kernel{}

{
}


template<
    typename Element,
    std::size_t rank>
void FocalMeanBenchmarkModel<Element, rank>::preprocess()
{
    _state = uniform(
        Array{this->array_shape(), this->partition_shape()},
        Element{0}, std::numeric_limits<Element>::max());

    lue_assert(_state.shape() == this->array_shape());

    _kernel = lue::box_kernel<bool, rank>(1, true);

    hpx::cout << describe(_state) << hpx::endl;


    // Optionally, write state -------------------------------------------------
    bool const write_info{false};

    if(write_info)
    {
        namespace ldm = lue::data_model;
        namespace lh5 = lue::hdf5;

        // using Shape = typename BenchmarkModel<rank>::Shape;
        using ConstantRasterView =
            ldm::constant::RasterView<std::shared_ptr<ldm::Dataset>>;

        std::string const dataset_pathname{"focal_mean.lue"};
        auto dataset_ptr{std::make_shared<ldm::Dataset>(
            ldm::create_dataset(dataset_pathname))};

        // Shape const array_shape{this->array_shape()};
        lh5::Shape array_shape(rank);
        std::copy(this->array_shape().begin(), this->array_shape().end(), array_shape.begin());

        std::string const phenomenon_name{"area"};
        std::string const constant_property_set_name{"constant"};

        ConstantRasterView constant_raster_view{
            ldm::constant::create_raster_view(
                dataset_ptr,
                phenomenon_name, constant_property_set_name,
                array_shape,
                {0.0, 0.0, double(array_shape[1]), double(array_shape[0])})};

        PartitionedArray<std::uint32_t, 2> locality_id{
            lue::locality_id(_state)};
        PartitionedArray<std::uint64_t, 2> array_partition_id{
            lue::array_partition_id(_state)};
        PartitionedArray<std::uint64_t, 2> nr_elements_per_locality{
            zonal_sum(array_like<std::uint64_t>(_state, 1), locality_id)};

        ConstantRasterView::Layer locality_id_layer{
            constant_raster_view.add_layer<std::uint32_t>(
                "locality_id")};
        ConstantRasterView::Layer array_partition_id_layer{
            constant_raster_view.add_layer<std::uint32_t>(
                "array_partition_id")};
        ConstantRasterView::Layer nr_elements_per_locality_layer{
            constant_raster_view.add_layer<std::uint32_t>(
                "nr_elements_per_locality")};

        std::vector<hpx::future<void>> written;
        written.push_back(
            write(locality_id, locality_id_layer));
        written.push_back(
            write(array_partition_id, array_partition_id_layer));
        written.push_back(
            write(nr_elements_per_locality, nr_elements_per_locality_layer));
        hpx::wait_all_n(written.begin(), written.size());
    }
}


template<
    typename Element,
    std::size_t rank>
void FocalMeanBenchmarkModel<Element, rank>::initialize()
{
    hpx::cout << '[' << hpx::flush;
}


template<
    typename Element,
    std::size_t rank>
void FocalMeanBenchmarkModel<Element, rank>::simulate(
    Count const /* time_step */)
{
    _state = focal_mean(_state, _kernel);
    hpx::cout << '.' << hpx::flush;
}


template<
    typename Element,
    std::size_t rank>
void FocalMeanBenchmarkModel<Element, rank>::terminate()
{
    hpx::wait_all_n(_state.begin(), _state.nr_partitions());
    hpx::cout << "]\n" << hpx::flush;
}


template<
    typename Element,
    std::size_t rank>
void FocalMeanBenchmarkModel<Element, rank>::postprocess()
{
    this->set_result(AlgorithmBenchmarkResult{_state.partitions().shape()});
}


// template<
//     typename Element,
//     std::size_t rank>
// AlgorithmBenchmarkResult focal_mean(
//     Task const& task,
//     std::size_t const max_tree_depth)
// {
//     lue_assert(max_tree_depth > 0);
// 
//     using Array = PartitionedArray<Element, rank>;
//     using Shape = typename Array::Shape;
// 
//     Shape shape;
//     std::copy(
//         task.array_shape().begin(), task.array_shape().end(),
//         shape.begin());
// 
//     Shape partition_shape;
//     std::copy(
//         task.partition_shape().begin(), task.partition_shape().end(),
//         partition_shape.begin());
// 
//     // → Create initial array
//     Array state{shape, partition_shape};
//     hpx::cout << describe(state) << hpx::endl;
// 
//     AlgorithmBenchmarkResult result{state.partitions().shape()};
// 
//     lue_assert(state.shape() == shape);
// 
//     state = uniform(state, Element{0}, std::numeric_limits<Element>::max());
// 
//     auto const kernel = lue::box_kernel<bool, rank>(1, true);
// 
//     hpx::lcos::local::sliding_semaphore semaphore{
//         static_cast<std::int64_t>(max_tree_depth)};
// 
//     for(std::size_t i = 0; i < task.nr_time_steps(); ++i) {
// 
//         // Wait if there are more than max_tree_depth iterations in flight
//         semaphore.wait(i);
// 
//         state = focal_mean(state, kernel);
// 
//         hpx::cout << '.' << hpx::flush;
// 
//         // Attach a continuation to the state at the current time
//         // step. Once it is finished, signal the semaphore so it knowns
//         // that we can have another iteration in flight.
//         hpx::when_all_n(state.begin(), state.nr_partitions()).then(
//             hpx::launch::sync,
//             [&semaphore, i](
//                 auto const&)
//             {
//                 semaphore.signal(i);
//             });
//     }
// 
//     hpx::cout << "!" << hpx::flush;
// 
//     hpx::wait_all_n(state.begin(), state.nr_partitions());
// 
//     hpx::cout << hpx::endl;
// 
//     return result;
// }

}  // namespace benchmark
}  // namespace lue


auto setup_benchmark(
    int /* argc */,
    char* /* argv */[],
    lue::benchmark::Environment const& environment,
    lue::benchmark::Task const& task)
{
    auto callable = [](
        lue::benchmark::Environment const& /* environment */,
        lue::benchmark::Task const& task)
    {
        return lue::benchmark::FocalMeanBenchmarkModel<double, 2>{task};
    };

    return lue::benchmark::ModelBenchmark<
            decltype(callable), lue::AlgorithmBenchmarkResult>{
        std::move(callable), environment, task};
}


LUE_CONFIGURE_HPX_BENCHMARK()

#include "wildfire_benchmark_model.hpp"
#include "lue/framework/algorithm/less_than.hpp"
#include "lue/framework/algorithm/uniform.hpp"


namespace lue {
namespace benchmark {
namespace detail {

class Model:
    public WildfireModelBase
{

public:

    Model(NominalRasterPtr const& state):

        WildfireModelBase{state}

    {
    }

    Model(Model const&)=default;

    Model(Model&&)=default;

    ~Model()=default;

    Model& operator=(Model const&)=default;

    Model& operator=(Model&&)=default;

private:

    BooleanRaster initial_fire() const
    {
        return uniform(state(), 0.0, 1.0) < 1e-5;
    }

};

}  // namespace detail


WildfireBenchmarkModel::WildfireBenchmarkModel(
    Environment const& /* environment */,
    Task const& task,
    std::size_t const max_tree_depth):

    BenchmarkModel<WildfireModelBase::NominalElement, 2>{task, max_tree_depth},
    _model{std::make_unique<detail::Model>(state_ptr())}

{
}


WildfireBenchmarkModel::~WildfireBenchmarkModel()
{
    // Not =default because then the forward declared detail::Model icw
    // std::unique_ptr won't compile.
}


void WildfireBenchmarkModel::do_initialize()
{
    _model->initialize();
}


void WildfireBenchmarkModel::do_simulate(
    Count const time_step)
{
    _model->simulate(time_step);
}

}  // namespace benchmark
}  // namespace lue

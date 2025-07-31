#include "lue/framework/algorithm/operator.hpp"
#include "lue/framework/algorithm/value_policies/add.hpp"
#include "lue/framework/algorithm/value_policies/divide.hpp"
#include "lue/framework/algorithm/value_policies/subtract.hpp"
#include "lue/framework/algorithm/value_policies/uniform.hpp"
#include "lue/framework/core/component.hpp"
#include <hpx/hpx_init.hpp>


auto hpx_main([[maybe_unused]] int argc, [[maybe_unused]] char* argv[]) -> int
{
    using namespace lue::value_policies;

    using Element = float;
    lue::Rank const rank{2};
    using Array = lue::PartitionedArray<Element, rank>;
    using Shape = lue::ShapeT<Array>;

    Shape const array_shape{40000, 40000};
    Shape const partition_shape{2000, 2000};
    Element const min{0};
    Element const max{1};

    auto const near_infrared = uniform(array_shape, partition_shape, min, max);
    auto const red = uniform(array_shape, partition_shape, min, max);
    auto const ndvi = (near_infrared - red) / (near_infrared + red);

    int hpx_status = hpx::finalize();

    return hpx_status;
}


auto main(int argc, char* argv[]) -> int
{
    std::vector<std::string> const cfg{};

    hpx::init_params params{};
    params.cfg = {cfg};

    return hpx::init(argc, argv, params);
}

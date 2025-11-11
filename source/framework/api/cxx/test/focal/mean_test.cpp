#define BOOST_TEST_MODULE lue framework api cxx focal mean
#include "lue/framework/api/cxx/focal/mean.hpp"
#include "lue/framework/test/hpx_unit_test.hpp"
#include "lue/framework.hpp"


BOOST_AUTO_TEST_CASE(focal_mean_raster_kernel)
{
    using Element = lue::FloatingPointElement<0>;
    using Weight = lue::BooleanElement;
    using Array = lue::PartitionedArray<Element, 2>;
    using Kernel = lue::Kernel<Weight, 2>;

    lue::api::Field const raster = Array{};
    lue::api::BooleanKernel const kernel = Kernel{};

    lue::api::Field result = lue::api::focal_mean(raster, kernel);

    BOOST_CHECK(std::holds_alternative<Array>(result.variant()));
}

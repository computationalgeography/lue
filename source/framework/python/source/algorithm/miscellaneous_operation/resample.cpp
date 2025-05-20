#include "lue/framework/algorithm/value_policies/resample.hpp"
#include "lue/framework.hpp"
#include "lue/py/bind.hpp"
#include <pybind11/pybind11.h>


using namespace pybind11::literals;


namespace lue::framework {
    namespace {

        class Binder
        {

            public:

                template<Arithmetic Element>
                static void bind(pybind11::module& module)
                {
                    Rank const rank{2};

                    module.def(
                        "downscale",
                        [](PartitionedArray<Element, rank> const& array,
                           Count const count,
                           DownscaleStrategy const strategy)

                        { return value_policies::downscale(array, count, strategy); },
                        "array"_a,
                        "count"_a,
                        "strategy"_a);
                }
        };


    }  // Anonymous namespace


    void bind_resample(pybind11::module& module)
    {
        pybind11::enum_<DownscaleStrategy>(module, "DownscaleStrategy")
            .value("assign", DownscaleStrategy::assign);

        bind<Binder, ArithmeticElements>(module);
    }

}  // namespace lue::framework

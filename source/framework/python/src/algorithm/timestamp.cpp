#include "lue/framework/algorithm/timestamp.hpp"
#include "lue/framework.hpp"
#include "lue/py/bind.hpp"


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
                        "timestamp",
                        [](PartitionedArray<Element, rank> const& array) { return timestamp(array); });
                }
        };

    }  // Anonymous namespace


    void bind_timestamp(pybind11::module& module)
    {
        static_assert(
            lue::arithmetic_element_supported<ClockTick>,
            "Reconfigure: clock ticks are represented by "
            "std::chrono::high_resolution_clock::duration::rep elements "
            "but LUE is built without support for that type");

        bind<Binder, ArithmeticElements>(module);
    }

}  // namespace lue::framework

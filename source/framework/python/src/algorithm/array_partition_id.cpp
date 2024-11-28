#include "lue/framework/algorithm/array_partition_id.hpp"
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
                        "array_partition_id",
                        [](PartitionedArray<Element, rank> const& array)
                        { return array_partition_id<IDElement>(array); });
                }
        };

    }  // Anonymous namespace


    void bind_array_partition_id(pybind11::module& module)
    {
        bind<Binder, ArithmeticElements>(module);
    }

}  // namespace lue::framework

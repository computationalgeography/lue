#include "lue/framework/algorithm/value_policies/global_operation.hpp"
#include "bind.hpp"
#include "lue/framework.hpp"


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
                        "maximum",
                        [](PartitionedArray<Element, rank> const& array) -> Scalar<Element>
                        { return value_policies::maximum(array); });

                    module.def(
                        "minimum",
                        [](PartitionedArray<Element, rank> const& array) -> Scalar<Element>
                        { return value_policies::minimum(array); });

                    module.def(
                        "sum",
                        [](PartitionedArray<Element, rank> const& array) -> Scalar<Element>
                        { return value_policies::sum(array); });
                }
        };

    }  // Anonymous namespace


    void bind_global_operation(pybind11::module& module)
    {
        bind<Binder, ArithmeticElements>(module);
    }

}  // namespace lue::framework

#include "lue/framework/algorithm/value_policies/all.hpp"
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
                    using namespace lue::value_policies;
                    Rank const rank{2};
                    using Array = PartitionedArray<Element, rank>;

                    module.def("all", [](Array const& array) { return all(array); });
                }
        };

    }  // Anonymous namespace


    void bind_all(pybind11::module& module)
    {
        bind<Binder, ArithmeticElements>(module);
    }

}  // namespace lue::framework

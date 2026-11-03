#include "lue/framework/algorithm/value_policies/any.hpp"
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

                    module.def("any", [](Array const& array) { return any(array); });
                }
        };

    }  // Anonymous namespace


    void bind_any(pybind11::module& module)
    {
        bind<Binder, IntegralElements>(module);
    }

}  // namespace lue::framework

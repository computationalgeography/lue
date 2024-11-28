#include "lue/framework/algorithm/value_policies/nearest_integer.hpp"
#include "lue/framework.hpp"
#include "lue/py/bind.hpp"


namespace lue::framework {
    namespace {

        class Binder
        {

            public:

                template<std::floating_point Element>
                static void bind(pybind11::module& module)
                {
                    using namespace lue::value_policies;
                    Rank const rank{2};
                    using Array = PartitionedArray<Element, rank>;
                    using Scalar = Scalar<Element>;
                    using Value = Element;

                    module.def("ceil", [](Array const& array) { return ceil(array); });
                    module.def("ceil", [](Scalar const& scalar) { return ceil(scalar); });
                    module.def("ceil", [](Value const value) { return ceil(value); });

                    module.def("floor", [](Array const& array) { return floor(array); });
                    module.def("floor", [](Scalar const& scalar) { return floor(scalar); });
                    module.def("floor", [](Value const value) { return floor(value); });

                    module.def("round", [](Array const& array) { return round(array); });
                    module.def("round", [](Scalar const& scalar) { return round(scalar); });
                    module.def("round", [](Value const value) { return round(value); });
                }
        };

    }  // Anonymous namespace


    void bind_nearest_integer(pybind11::module& module)
    {
        bind<Binder, FloatingPointElements>(module);
    }

}  // namespace lue::framework

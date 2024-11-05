#include "lue/framework/algorithm/value_policies/trigonometry.hpp"
#include "bind.hpp"
#include "lue/framework.hpp"


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

                    module.def("acos", [](Array const& array) { return acos(array); });
                    module.def("acos", [](Scalar const& scalar) { return acos(scalar); });
                    module.def("acos", [](Value const value) { return acos(value); });

                    module.def("asin", [](Array const& array) { return asin(array); });
                    module.def("asin", [](Scalar const& scalar) { return asin(scalar); });
                    module.def("asin", [](Value const value) { return asin(value); });

                    module.def("atan", [](Array const& array) { return atan(array); });
                    module.def("atan", [](Scalar const& scalar) { return atan(scalar); });
                    module.def("atan", [](Value const value) { return atan(value); });

                    module.def(
                        "atan2",
                        [](Array const& array1, Array const& array2) { return atan2(array1, array2); });

                    module.def("cos", [](Array const& array) { return cos(array); });
                    module.def("cos", [](Scalar const& scalar) { return cos(scalar); });
                    module.def("cos", [](Value const value) { return cos(value); });

                    module.def("sin", [](Array const& array) { return sin(array); });
                    module.def("sin", [](Scalar const& scalar) { return sin(scalar); });
                    module.def("sin", [](Value const value) { return sin(value); });

                    module.def("tan", [](Array const& array) { return tan(array); });
                    module.def("tan", [](Scalar const& scalar) { return tan(scalar); });
                    module.def("tan", [](Value const value) { return tan(value); });
                }
        };

    }  // Anonymous namespace


    void bind_trigonometry(pybind11::module& module)
    {
        bind<Binder, FloatingPointElements>(module);
    }

}  // namespace lue::framework

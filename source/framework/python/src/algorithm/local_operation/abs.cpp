#include "lue/framework/algorithm/value_policies/abs.hpp"
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
                    using namespace lue::value_policies;
                    Rank const rank{2};
                    using Array = PartitionedArray<Element, rank>;
                    using Scalar = Scalar<Element>;
                    using Value = Element;

                    module.def("abs", [](Array const& array) { return abs(array); });
                    module.def("abs", [](Scalar const& scalar) { return abs(scalar); });
                    module.def("abs", [](Value const value) { return abs(value); });
                }
        };

    }  // Anonymous namespace

    void bind_abs(pybind11::module& module)
    {
        bind<Binder, SignedArithmeticElements>(module);
    }

}  // namespace lue::framework

#include "lue/framework/algorithm/value_policies/valid.hpp"
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

                    module.def("valid", [](Array const& array) { return valid<BooleanElement>(array); });
                    module.def("valid", [](Scalar const& scalar) { return valid<BooleanElement>(scalar); });
                    module.def("valid", [](Value const value) { return valid<BooleanElement>(value); });
                }
        };

    }  // Anonymous namespace


    void bind_valid(pybind11::module& module)
    {
        bind<Binder, ArithmeticElements>(module);
    }

}  // namespace lue::framework

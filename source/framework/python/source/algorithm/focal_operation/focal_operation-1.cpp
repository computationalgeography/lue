#include "array_to_kernel.hpp"
#include "lue/framework/algorithm/value_policies/focal_operation.hpp"
#include "lue/framework.hpp"
#include "lue/py/bind.hpp"


using namespace pybind11::literals;


namespace lue::framework {
    namespace {

        class ArithmeticElementBinder
        {

            public:

                template<Arithmetic Element>
                static void bind(pybind11::module& module)
                {
                    Rank const rank{2};

                    module.def(
                        "focal_maximum",
                        [](PartitionedArray<Element, rank> const& array,
                           pybind11::array_t<BooleanElement> const& kernel)
                        {
                            return value_policies::focal_maximum(
                                array, array_to_kernel<BooleanElement, rank>(kernel));
                        },
                        "array"_a,
                        "kernel"_a.noconvert());

                    module.def(
                        "focal_minimum",
                        [](PartitionedArray<Element, rank> const& array,
                           pybind11::array_t<BooleanElement> const& kernel)
                        {
                            return value_policies::focal_minimum(
                                array, array_to_kernel<BooleanElement, rank>(kernel));
                        },
                        "array"_a,
                        "kernel"_a.noconvert());

                    module.def(
                        "focal_sum",
                        [](PartitionedArray<Element, rank> const& array,
                           pybind11::array_t<BooleanElement> const& kernel)
                        {
                            return value_policies::focal_sum(
                                array, array_to_kernel<BooleanElement, rank>(kernel));
                        },
                        "array"_a,
                        "kernel"_a.noconvert());
                }
        };

    }  // Anonymous namespace


    void bind_focal1(pybind11::module& module)
    {
        bind<ArithmeticElementBinder, ArithmeticElements>(module);
    }

}  // namespace lue::framework

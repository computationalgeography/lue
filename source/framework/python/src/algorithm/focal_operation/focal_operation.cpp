#include "lue/framework/algorithm/value_policies/focal_operation.hpp"
#include "array_to_kernel.hpp"
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
                           pybind11::array_t<BooleanElement> const& kernel) {
                            return value_policies::focal_maximum(
                                array, array_to_kernel<BooleanElement, rank>(kernel));
                        },
                        "array"_a,
                        "kernel"_a.noconvert());

                    module.def(
                        "focal_minimum",
                        [](PartitionedArray<Element, rank> const& array,
                           pybind11::array_t<BooleanElement> const& kernel) {
                            return value_policies::focal_minimum(
                                array, array_to_kernel<BooleanElement, rank>(kernel));
                        },
                        "array"_a,
                        "kernel"_a.noconvert());

                    module.def(
                        "focal_sum",
                        [](PartitionedArray<Element, rank> const& array,
                           pybind11::array_t<BooleanElement> const& kernel) {
                            return value_policies::focal_sum(
                                array, array_to_kernel<BooleanElement, rank>(kernel));
                        },
                        "array"_a,
                        "kernel"_a.noconvert());
                }
        };


        class FloatingPointElementBinder
        {

            public:

                template<std::floating_point Element>
                static void bind(pybind11::module& module)
                {
                    Rank const rank{2};

                    module.def(
                        "aspect",
                        [](PartitionedArray<Element, rank> const& elevation)
                        { return value_policies::aspect(elevation); },
                        "elevation"_a);

                    module.def(
                        "focal_high_pass",
                        [](PartitionedArray<Element, rank> const& array,
                           pybind11::array_t<BooleanElement> const& kernel) {
                            return value_policies::focal_high_pass(
                                array, array_to_kernel<BooleanElement, rank>(kernel));
                        },
                        "array"_a,
                        "kernel"_a.noconvert());

                    module.def(
                        "focal_mean",
                        [](PartitionedArray<Element, rank> const& array,
                           pybind11::array_t<BooleanElement> const& kernel) {
                            return value_policies::focal_mean(
                                array, array_to_kernel<BooleanElement, rank>(kernel));
                        },
                        "array"_a,
                        "kernel"_a.noconvert());

                    module.def(
                        "slope",
                        [](PartitionedArray<Element, rank> const& elevation, Element const& cell_size)
                        { return value_policies::slope(elevation, cell_size); },
                        "elevation"_a,
                        "cell_size"_a);
                }
        };


        class IntegralElementBinder
        {

            public:

                template<std::integral Element>
                static void bind(pybind11::module& module)
                {
                    Rank const rank{2};

                    module.def(
                        "focal_diversity",
                        [](PartitionedArray<Element, rank> const& array,
                           pybind11::array_t<BooleanElement> const& kernel)
                        {
                            return value_policies::focal_diversity<CountElement>(
                                array, array_to_kernel<BooleanElement, rank>(kernel));
                        },
                        "array"_a,
                        "kernel"_a.noconvert());

                    module.def(
                        "focal_majority",
                        [](PartitionedArray<Element, rank> const& array,
                           pybind11::array_t<BooleanElement> const& kernel) {
                            return value_policies::focal_majority(
                                array, array_to_kernel<BooleanElement, rank>(kernel));
                        },
                        "array"_a,
                        "kernel"_a.noconvert());
                }
        };

    }  // Anonymous namespace


    void bind_focal(pybind11::module& module)
    {
        bind<ArithmeticElementBinder, ArithmeticElements>(module);
        bind<FloatingPointElementBinder, FloatingPointElements>(module);
        bind<IntegralElementBinder, IntegralElements>(module);
    }

}  // namespace lue::framework

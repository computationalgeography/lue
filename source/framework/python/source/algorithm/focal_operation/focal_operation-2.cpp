#include "array_to_kernel.hpp"
#include "lue/framework/algorithm/value_policies/focal_operation.hpp"
#include "lue/framework.hpp"
#include "lue/py/bind.hpp"


using namespace pybind11::literals;


namespace lue::framework {
    namespace {

        class FloatingPointElementBinder
        {

            public:

                template<std::floating_point Element>
                static void bind(pybind11::module& module)
                {
                    Rank const rank{2};

                    module.def(
                        "aspect",
                        [](PartitionedArray<Element, rank> const& elevation) -> auto
                        { return value_policies::aspect(elevation); },
                        "elevation"_a);

                    module.def(
                        "convolve",
                        [](PartitionedArray<Element, rank> const& array,
                           pybind11::array_t<Element> const& kernel) -> auto
                        { return value_policies::convolve(array, array_to_kernel<Element, rank>(kernel)); },
                        "array"_a,
                        "kernel"_a.noconvert());

                    module.def(
                        "focal_high_pass",
                        [](PartitionedArray<Element, rank> const& array,
                           pybind11::array_t<BooleanElement> const& kernel) -> auto
                        {
                            return value_policies::focal_high_pass(
                                array, array_to_kernel<BooleanElement, rank>(kernel));
                        },
                        "array"_a,
                        "kernel"_a.noconvert());

                    module.def(
                        "focal_mean",
                        [](PartitionedArray<Element, rank> const& array,
                           pybind11::array_t<BooleanElement> const& kernel) -> auto
                        {
                            return value_policies::focal_mean(
                                array, array_to_kernel<BooleanElement, rank>(kernel));
                        },
                        "array"_a,
                        "kernel"_a.noconvert());

                    module.def(
                        "slope",
                        [](PartitionedArray<Element, rank> const& elevation, Element const& cell_size) -> auto
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
                           pybind11::array_t<BooleanElement> const& kernel) -> auto
                        {
                            return value_policies::focal_diversity<CountElement>(
                                array, array_to_kernel<BooleanElement, rank>(kernel));
                        },
                        "array"_a,
                        "kernel"_a.noconvert());

                    module.def(
                        "focal_majority",
                        [](PartitionedArray<Element, rank> const& array,
                           pybind11::array_t<BooleanElement> const& kernel) -> auto
                        {
                            return value_policies::focal_majority(
                                array, array_to_kernel<BooleanElement, rank>(kernel));
                        },
                        "array"_a,
                        "kernel"_a.noconvert());
                }
        };

    }  // Anonymous namespace


    void bind_focal2(pybind11::module& module)
    {
        bind<FloatingPointElementBinder, FloatingPointElements>(module);
        bind<IntegralElementBinder, IntegralElements>(module);
    }

}  // namespace lue::framework

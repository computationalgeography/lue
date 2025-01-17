#include "lue/framework/algorithm/value_policies/zonal_operation.hpp"
#include "lue/framework.hpp"
#include "lue/py/bind.hpp"
#include <pybind11/numpy.h>


using namespace pybind11::literals;


namespace lue::framework {
    namespace {

        class Binder
        {

            public:

                template<std::integral ZoneElement>
                static void bind(pybind11::module& module)
                {
                    Rank const rank{2};

                    module.def(
                        "clump",
                        [](PartitionedArray<ZoneElement, rank> const& zones, Connectivity const connectivity)
                        { return value_policies::clump(zones, connectivity); },
                        "zones"_a,
                        "connectivity"_a);

                    module.def(
                        "zonal_area",
                        [](PartitionedArray<ZoneElement, rank> const& zones)
                        { return value_policies::zonal_area<CountElement>(zones); },
                        "zones"_a);
                }
        };


        class ArithmeticBinder
        {

            public:

                template<Arithmetic Element, std::integral ZoneElement>
                static void bind(pybind11::module& module)
                {
                    Rank const rank{2};

                    module.def(
                        "zonal_maximum",
                        [](PartitionedArray<Element, rank> const& array,
                           PartitionedArray<ZoneElement, rank> const& zones)
                        { return value_policies::zonal_maximum(array, zones); },
                        "array"_a,
                        "zones"_a);

                    module.def(
                        "zonal_minimum",
                        [](PartitionedArray<Element, rank> const& array,
                           PartitionedArray<ZoneElement, rank> const& zones)
                        { return value_policies::zonal_minimum(array, zones); },
                        "array"_a,
                        "zones"_a);

                    module.def(
                        "zonal_sum",
                        [](PartitionedArray<Element, rank> const& array,
                           PartitionedArray<ZoneElement, rank> const& zones)
                        { return value_policies::zonal_sum(array, zones); },
                        "array"_a,
                        "zones"_a);
                }
        };


        class DTypeBinder
        {

            public:

                template<std::integral ZoneElement>
                static void bind(pybind11::module& module)
                {
                    Rank const rank{2};

                    module.def(
                        "zonal_normal",
                        [](PartitionedArray<ZoneElement, rank> const& zones,
                           pybind11::object const& dtype_args)
                        {
                            pybind11::dtype const dtype{pybind11::dtype::from_args(dtype_args)};

                            // Switch on dtype and call a function that returns an array of the
                            // right value type
                            auto const kind = dtype.kind();
                            auto const size = dtype.itemsize();  // bytes
                            pybind11::object result;

                            switch (kind)
                            {
                                case 'f':
                                {
                                    // Floating-point
                                    switch (size)
                                    {
                                        case 4:
                                        {
                                            using OutputElement = float;

                                            if constexpr (arithmetic_element_supported<OutputElement>)
                                            {
                                                result = pybind11::cast(
                                                    value_policies::zonal_normal<OutputElement>(zones));
                                            }

                                            break;
                                        }
                                        case 8:
                                        {
                                            using OutputElement = double;

                                            if constexpr (arithmetic_element_supported<OutputElement>)
                                            {
                                                result = pybind11::cast(
                                                    value_policies::zonal_normal<OutputElement>(zones));
                                            }

                                            break;
                                        }
                                    }

                                    break;
                                }
                            }

                            if (!result)
                            {
                                throw std::runtime_error(
                                    fmt::format("Unsupported dtype (kind={}, itemsize={})", kind, size));
                            }

                            return result;
                        },
                        "zones"_a,
                        "dtype"_a);

                    module.def(
                        "zonal_uniform",
                        [](PartitionedArray<ZoneElement, rank> const& zones,
                           pybind11::object const& dtype_args)
                        {
                            pybind11::dtype const dtype{pybind11::dtype::from_args(dtype_args)};

                            // Switch on dtype and call a function that returns an array of the
                            // right value type
                            auto const kind = dtype.kind();
                            auto const size = dtype.itemsize();  // bytes
                            pybind11::object result;

                            switch (kind)
                            {
                                case 'f':
                                {
                                    // Floating-point
                                    switch (size)
                                    {
                                        case 4:
                                        {
                                            using OutputElement = float;

                                            if constexpr (arithmetic_element_supported<OutputElement>)
                                            {
                                                result = pybind11::cast(
                                                    value_policies::zonal_uniform<OutputElement>(zones));
                                            }

                                            break;
                                        }
                                        case 8:
                                        {
                                            using OutputElement = double;

                                            if constexpr (arithmetic_element_supported<OutputElement>)
                                            {
                                                result = pybind11::cast(
                                                    value_policies::zonal_uniform<OutputElement>(zones));
                                            }

                                            break;
                                        }
                                    }

                                    break;
                                }
                            }

                            if (!result)
                            {
                                throw std::runtime_error(
                                    fmt::format("Unsupported dtype (kind={}, itemsize={})", kind, size));
                            }

                            return result;
                        },
                        "zones"_a,
                        "dtype"_a);
                }
        };


        class FloatingPointBinder
        {

            public:

                template<std::floating_point Element, std::integral ZoneElement>
                static void bind(pybind11::module& module)
                {
                    Rank const rank{2};

                    module.def(
                        "zonal_mean",
                        [](PartitionedArray<Element, rank> const& array,
                           PartitionedArray<ZoneElement, rank> const& zones)
                        { return value_policies::zonal_mean(array, zones); },
                        "array"_a,
                        "zones"_a);
                }
        };


        class IntegralBinder
        {

            public:

                template<std::integral Element, std::integral ZoneElement>
                static void bind(pybind11::module& module)
                {
                    Rank const rank{2};

                    module.def(
                        "zonal_diversity",
                        [](PartitionedArray<Element, rank> const& array,
                           PartitionedArray<ZoneElement, rank> const& zones)
                        { return value_policies::zonal_diversity<CountElement>(array, zones); },
                        "array"_a,
                        "zones"_a);

                    module.def(
                        "zonal_majority",
                        [](PartitionedArray<Element, rank> const& array,
                           PartitionedArray<ZoneElement, rank> const& zones)
                        { return value_policies::zonal_majority(array, zones); },
                        "array"_a,
                        "zones"_a);
                }
        };

    }  // Anonymous namespace


    void bind_zonal_operation(pybind11::module& module)
    {
        pybind11::enum_<Connectivity>(module, "Connectivity")
            .value("diagonal", Connectivity::diagonal)
            .value("nondiagonal", Connectivity::nondiagonal)
            // .export_values()
            ;

        bind<Binder, ZoneElements>(module);
        bind<IntegralBinder, IntegralElements, ZoneElements>(module);
        bind<ArithmeticBinder, ArithmeticElements, ZoneElements>(module);
        bind<FloatingPointBinder, FloatingPointElements, ZoneElements>(module);
        bind<DTypeBinder, ZoneElements>(module);
    }

}  // namespace lue::framework

#include "lue/framework/algorithm/value_policies/normal.hpp"
#include "shape.hpp"
#include "lue/concept.hpp"
#include "lue/framework.hpp"
#include <pybind11/numpy.h>
#include <pybind11/stl.h>


using namespace pybind11::literals;


namespace lue::framework {
    namespace {

        template<typename Element, Rank rank>
        auto normal1(
            PartitionedArray<Element, rank> const& array,
            pybind11::object const& dtype_args,
            pybind11::object const& mean,
            pybind11::object const& stddev) -> pybind11::object
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
                                result = pybind11::cast(value_policies::normal(
                                    array,
                                    pybind11::cast<OutputElement>(mean),
                                    pybind11::cast<OutputElement>(stddev)));
                            }

                            break;
                        }
                        case 8:
                        {
                            using OutputElement = double;

                            if constexpr (arithmetic_element_supported<OutputElement>)
                            {
                                result = pybind11::cast(value_policies::normal(
                                    array,
                                    pybind11::cast<OutputElement>(mean),
                                    pybind11::cast<OutputElement>(stddev)));
                            }

                            break;
                        }
                    }

                    break;
                }
            }

            if (!result)
            {
                throw std::runtime_error(fmt::format("Unsupported dtype (kind={}, itemsize={})", kind, size));
            }

            return result;
        }


        template<Arithmetic Element>
        void bind(pybind11::module& module)
        {
            Rank const rank{2};

            module.def("normal", normal1<Element, rank>);
        }


        template<TupleLike Elements, std::size_t idx>
        void bind(pybind11::module& module) requires(idx == 0)
        {
            bind<std::tuple_element_t<idx, Elements>>(module);
        }


        template<TupleLike Elements, std::size_t idx>
        void bind(pybind11::module& module) requires(idx > 0)
        {
            bind<std::tuple_element_t<idx, Elements>>(module);
            bind<Elements, idx - 1>(module);
        }


        template<TupleLike Elements>
        void bind(pybind11::module& module)
        {
            bind<Elements, std::tuple_size_v<Elements> - 1>(module);
        }


        // Step 3: Call the algorithm
        template<typename Element, Rank rank>
        pybind11::object normal(
            StaticShape<rank> const& array_shape,
            StaticShape<rank> const& partition_shape,
            pybind11::object const& mean,
            pybind11::object const& stddev)
        {
            return pybind11::cast(value_policies::normal(
                array_shape,
                partition_shape,
                pybind11::cast<Element>(mean),
                pybind11::cast<Element>(stddev)));
        }


        // Step 2: Determine the type of the element type of the result array
        template<Rank rank>
        pybind11::object normal(
            StaticShape<rank> const& array_shape,
            StaticShape<rank> const& partition_shape,
            pybind11::dtype const& dtype,
            pybind11::object const& mean,
            pybind11::object const& stddev)
        {
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
                            using Element = float;

                            if constexpr (arithmetic_element_supported<Element>)
                            {
                                result = normal<Element, rank>(array_shape, partition_shape, mean, stddev);
                            }

                            break;
                        }
                        case 8:
                        {
                            using Element = double;

                            if constexpr (arithmetic_element_supported<Element>)
                            {
                                result = normal<Element, rank>(array_shape, partition_shape, mean, stddev);
                            }

                            break;
                        }
                    }

                    break;
                }
            }

            if (!result)
            {
                throw std::runtime_error(fmt::format("Unsupported dtype (kind={}, itemsize={})", kind, size));
            }

            return result;
        }


        // Step 1: Convert from Python types to C++ types and validate contents
        auto normal_py(
            pybind11::tuple const& array_shape,
            pybind11::object const& dtype_args,
            pybind11::object const& mean,
            pybind11::object const& stddev,
            std::optional<pybind11::tuple> const& partition_shape) -> pybind11::object
        {
            DynamicShape const dynamic_array_shape{tuple_to_shape(array_shape)};
            pybind11::dtype const dtype{pybind11::dtype::from_args(dtype_args)};
            Rank const rank{dynamic_array_shape.size()};

            if (partition_shape)
            {
                DynamicShape const dynamic_partition_shape{tuple_to_shape(*partition_shape)};

                if (dynamic_array_shape.size() != dynamic_partition_shape.size())
                {
                    throw std::runtime_error(fmt::format(
                        "Rank of array shape and partition shape must be equal ({} != {})",
                        dynamic_array_shape.size(),
                        dynamic_partition_shape.size()));
                }
            }

            pybind11::object result;

            verify_rank_supported(rank);

            if constexpr (rank_supported(1))
            {
                if (rank == 1)
                {
                    StaticShape<1> const static_array_shape{
                        dynamic_shape_to_static_shape<1>(dynamic_array_shape)};
                    StaticShape<1> static_partition_shape{};

                    if (partition_shape)
                    {
                        static_partition_shape =
                            dynamic_shape_to_static_shape<1>(tuple_to_shape(*partition_shape));
                    }
                    else
                    {
                        static_partition_shape = default_partition_shape(static_array_shape);
                    }

                    result = normal<1>(static_array_shape, static_partition_shape, dtype, mean, stddev);
                }
            }

            if constexpr (rank_supported(2))
            {
                if (rank == 2)
                {
                    StaticShape<2> const static_array_shape{
                        dynamic_shape_to_static_shape<2>(dynamic_array_shape)};
                    StaticShape<2> static_partition_shape{};

                    if (partition_shape)
                    {
                        static_partition_shape =
                            dynamic_shape_to_static_shape<2>(tuple_to_shape(*partition_shape));
                    }
                    else
                    {
                        static_partition_shape = default_partition_shape(static_array_shape);
                    }

                    result = normal<2>(static_array_shape, static_partition_shape, dtype, mean, stddev);
                }
            }

            lue_hpx_assert(result);

            return result;
        }

    }  // Anonymous namespace


    void bind_normal(pybind11::module& module)
    {
        bind<ArithmeticElements>(module);

        module.def(
            "normal",
            normal_py,
            R"(
    Create new array, filled with normally distributed random values

    :param tuple array_shape: Shape of the array
    :param numpy.dtype dtype: Type of the array elements
    :param mean: Mean of normal distribution
    :param stddev: Standard deviation of normal distribution
    :param tuple partition_shape: Shape of the array partitions. When not
        passed in, a default shape will be used which might not result in the
        best performance and scalability.
    :rtype: PartitionedArray specialization

    The type of the array returned depends on the rank of the array and
    the type of the array elements.
)",
            "array_shape"_a,
            "dtype"_a,
            "mean"_a,
            "stddev"_a,
            pybind11::kw_only(),
            "partition_shape"_a = std::optional<pybind11::tuple>{},
            pybind11::return_value_policy::move);
    }

}  // namespace lue::framework

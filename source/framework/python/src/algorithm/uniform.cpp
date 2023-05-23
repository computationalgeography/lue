#include "lue/framework/algorithm/value_policies/uniform.hpp"
#include "shape.hpp"
#include <pybind11/numpy.h>
#include <pybind11/stl.h>


// std::uint8_t is not supported:
// https://stackoverflow.com/questions/31460733/why-arent-stduniform-int-distributionuint8-t-and-stduniform-int-distri


namespace lue::framework {
    namespace {

        template<typename Element, Rank rank>
        pybind11::object uniform1(
            PartitionedArray<Element, rank> const& array,
            pybind11::object const& dtype_args,
            pybind11::object const& min_value,
            pybind11::object const& max_value)
        {
            pybind11::dtype const dtype{pybind11::dtype::from_args(dtype_args)};

            // Switch on dtype and call a function that returns an array of the
            // right value type
            auto const kind = dtype.kind();
            auto const size = dtype.itemsize();  // bytes
            pybind11::object result;

            switch (kind)
            {
                case 'i':
                {
                    // Signed integer
                    switch (size)
                    {
                        case 4:
                        {
                            result = pybind11::cast(value_policies::uniform(
                                array,
                                pybind11::cast<std::int32_t>(min_value),
                                pybind11::cast<std::int32_t>(max_value)));
                            break;
                        }
                        case 8:
                        {
                            result = pybind11::cast(value_policies::uniform(
                                array,
                                pybind11::cast<std::int64_t>(min_value),
                                pybind11::cast<std::int64_t>(max_value)));
                            break;
                        }
                    }

                    break;
                }
                case 'u':
                {
                    // Unsigned integer
                    switch (size)
                    {
                        case 4:
                        {
                            result = pybind11::cast(value_policies::uniform(
                                array,
                                pybind11::cast<std::uint32_t>(min_value),
                                pybind11::cast<std::uint32_t>(max_value)));
                            break;
                        }
                        case 8:
                        {
                            result = pybind11::cast(value_policies::uniform(
                                array,
                                pybind11::cast<std::uint64_t>(min_value),
                                pybind11::cast<std::uint64_t>(max_value)));
                            break;
                        }
                    }

                    break;
                }
                case 'f':
                {
                    // Floating-point
                    switch (size)
                    {
                        case 4:
                        {
                            result = pybind11::cast(value_policies::uniform(
                                array, pybind11::cast<float>(min_value), pybind11::cast<float>(max_value)));
                            break;
                        }
                        case 8:
                        {
                            result = pybind11::cast(value_policies::uniform(
                                array, pybind11::cast<double>(min_value), pybind11::cast<double>(max_value)));
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


        // Step 3: Call the algorithm
        template<typename Element, Rank rank>
        pybind11::object uniform(
            StaticShape<rank> const& array_shape,
            StaticShape<rank> const& partition_shape,
            pybind11::object const& min_value,
            pybind11::object const& max_value)
        {
            return pybind11::cast(value_policies::uniform(
                array_shape,
                partition_shape,
                pybind11::cast<Element>(min_value),
                pybind11::cast<Element>(max_value)));
        }


        // Step 2: Determine the type of the element type of the result array
        template<Rank rank>
        pybind11::object uniform(
            StaticShape<rank> const& array_shape,
            StaticShape<rank> const& partition_shape,
            pybind11::dtype const& dtype,
            pybind11::object const& min_value,
            pybind11::object const& max_value)
        {
            // Switch on dtype and call a function that returns an array of the
            // right value type
            auto const kind = dtype.kind();
            auto const size = dtype.itemsize();  // bytes
            pybind11::object result;

            switch (kind)
            {
                case 'i':
                {
                    // Signed integer
                    switch (size)
                    {
                        case 4:
                        {
                            result = uniform<std::int32_t, rank>(
                                array_shape, partition_shape, min_value, max_value);
                            break;
                        }
                        case 8:
                        {
                            result = uniform<std::int64_t, rank>(
                                array_shape, partition_shape, min_value, max_value);
                            break;
                        }
                    }

                    break;
                }
                case 'u':
                {
                    // Unsigned integer
                    switch (size)
                    {
                        case 4:
                        {
                            result = uniform<std::uint32_t, rank>(
                                array_shape, partition_shape, min_value, max_value);
                            break;
                        }
                        case 8:
                        {
                            result = uniform<std::uint64_t, rank>(
                                array_shape, partition_shape, min_value, max_value);
                            break;
                        }
                    }

                    break;
                }
                case 'f':
                {
                    // Floating-point
                    switch (size)
                    {
                        case 4:
                        {
                            result = uniform<float, rank>(array_shape, partition_shape, min_value, max_value);
                            break;
                        }
                        case 8:
                        {
                            result =
                                uniform<double, rank>(array_shape, partition_shape, min_value, max_value);
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
        pybind11::object uniform2(
            DynamicShape const& array_shape,
            DynamicShape const& partition_shape,
            pybind11::object const& dtype_args,
            pybind11::object const& min_value,
            pybind11::object const& max_value)
        {
            pybind11::dtype const dtype{pybind11::dtype::from_args(dtype_args)};

            if (array_shape.size() != partition_shape.size())
            {
                throw std::runtime_error(fmt::format(
                    "Rank of array shape and partition shape must be equal ({} != {})",
                    array_shape.size(),
                    partition_shape.size()));
            }

            Rank const rank{array_shape.size()};
            pybind11::object result;

            if (rank == 1)
            {
                result = uniform<1>(
                    dynamic_shape_to_static_shape<1>(array_shape),
                    dynamic_shape_to_static_shape<1>(partition_shape),
                    dtype,
                    min_value,
                    max_value);
            }
            else if (rank == 2)
            {
                result = uniform<2>(
                    dynamic_shape_to_static_shape<2>(array_shape),
                    dynamic_shape_to_static_shape<2>(partition_shape),
                    dtype,
                    min_value,
                    max_value);
            }
            else
            {
                throw std::runtime_error("Currently only arrays of rank 1 or 2 are supported");
            }

            lue_hpx_assert(result);

            return result;
        }

    }  // Anonymous namespace


    void bind_uniform(pybind11::module& module)
    {
        module.def("uniform", uniform1<std::uint8_t, 2>);
        module.def("uniform", uniform1<std::uint32_t, 2>);
        module.def("uniform", uniform1<std::uint64_t, 2>);
        module.def("uniform", uniform1<std::int32_t, 2>);
        module.def("uniform", uniform1<std::int64_t, 2>);
        module.def("uniform", uniform1<float, 2>);
        module.def("uniform", uniform1<double, 2>);

        module.def("uniform", uniform2);
    }

}  // namespace lue::framework

#include "lue/framework/algorithm/value_policies/zonal_normal.hpp"
#include <pybind11/numpy.h>


using namespace pybind11::literals;


namespace lue::framework {
    namespace {

        template<
            typename Zone,
            Rank rank>
        pybind11::object zonal_normal(
            PartitionedArray<Zone, rank> const& zones,
            pybind11::object const& dtype_args)
        {
            pybind11::dtype const dtype{pybind11::dtype::from_args(dtype_args)};

            // Switch on dtype and call a function that returns an array of the
            // right value type
            auto const kind = dtype.kind();
            auto const size = dtype.itemsize();  // bytes
            pybind11::object result;

            switch(kind)
            {
                case 'f':
                {
                    // Floating-point
                    switch(size)
                    {
                        case 4: {
                            result = pybind11::cast(value_policies::zonal_normal<float>(zones));
                            break;
                        }
                        case 8: {
                            result = pybind11::cast(value_policies::zonal_normal<double>(zones));
                            break;
                        }
                    }

                    break;
                }
            }

            if(!result)
            {
                throw std::runtime_error(fmt::format(
                    "Operation expects dtype representing float32 or float64, "
                    "but got: kind={}, itemsize={}",
                    kind, size));
            }

            return result;
        }

    }  // Anonymous namespace


    void bind_zonal_normal(
        pybind11::module& module)
    {
        module.def("zonal_normal", zonal_normal<std::uint8_t, 2>, "zones"_a, "dtype"_a);
        module.def("zonal_normal", zonal_normal<std::uint32_t, 2>, "zones"_a, "dtype"_a);
        module.def("zonal_normal", zonal_normal<std::uint64_t, 2>, "zones"_a, "dtype"_a);
        module.def("zonal_normal", zonal_normal<std::int32_t, 2>, "zones"_a, "dtype"_a);
        module.def("zonal_normal", zonal_normal<std::int64_t, 2>, "zones"_a, "dtype"_a);
    }

}  // namespace lue::framework

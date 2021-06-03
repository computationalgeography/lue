#include "lue/framework/io/write_into.hpp"
#include "lue/data_model/hl/raster_view.hpp"
#include <pybind11/pybind11.h>


namespace lh5 = lue::hdf5;
namespace ldm = lue::data_model;
using namespace pybind11::literals;


namespace lue::framework {
    namespace {

        template<
            typename Element,
            Rank rank>
        hpx::future<void> write_constant_array(
            [[maybe_unused]] PartitionedArray<Element, rank> const& array,
            [[maybe_unused]] std::string const& array_pathname)
        {
            auto const [dataset_pathname, phenomenon_name, property_set_name, layer_name] =
                parse_array_pathname(array_pathname);
            auto const [object_id, file_datatype] = ldm::constant::probe_raster(
                dataset_pathname, phenomenon_name, property_set_name, layer_name);

            if(rank != 2)
            {
                throw std::runtime_error(
                    fmt::format("Unsupported rank ({}). Currently only rank 2 is supported", rank));
            }

            using Policies = policy::write_into::DefaultValuePolicies<Element>;

            return write(Policies{}, array, array_pathname, object_id);
        }


        template<
            typename Element,
            Rank rank>
        hpx::future<void> write_variable_array(
            [[maybe_unused]] PartitionedArray<Element, rank> const& array,
            [[maybe_unused]] Index const time_step_idx,
            [[maybe_unused]] std::string const& array_pathname)
        {
            auto const [dataset_pathname, phenomenon_name, property_set_name, layer_name] =
                parse_array_pathname(array_pathname);
            auto const [object_id, file_datatype] = ldm::variable::probe_raster(
                dataset_pathname, phenomenon_name, property_set_name, layer_name);

            if(rank != 2)
            {
                throw std::runtime_error(
                    fmt::format("Unsupported rank ({}). Currently only rank 2 is supported", rank));
            }

            using Policies = policy::write_into::DefaultValuePolicies<Element>;

            return write(Policies{}, array, array_pathname, object_id, time_step_idx);
        }

    }  // Anonymous namespace


    void bind_write_array(
        pybind11::module& module)
    {
        module.def("write_array", write_constant_array<uint8_t, 2>);
        module.def("write_array", write_constant_array<uint32_t, 2>);
        module.def("write_array", write_constant_array<uint64_t, 2>);
        module.def("write_array", write_constant_array<int32_t, 2>);
        module.def("write_array", write_constant_array<int64_t, 2>);
        module.def("write_array", write_constant_array<float, 2>);
        module.def("write_array", write_constant_array<double, 2>);

        module.def("write_array", write_variable_array<uint8_t, 2>);
        module.def("write_array", write_variable_array<uint32_t, 2>);
        module.def("write_array", write_variable_array<uint64_t, 2>);
        module.def("write_array", write_variable_array<int32_t, 2>);
        module.def("write_array", write_variable_array<int64_t, 2>);
        module.def("write_array", write_variable_array<float, 2>);
        module.def("write_array", write_variable_array<double, 2>);
    }

}  // namespace lue::framework

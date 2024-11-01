#include "lue/framework/io/write_into.hpp"
#include "lue/data_model/hl/raster_view.hpp"
#include "lue/concept.hpp"
#include "lue/framework.hpp"
#include <pybind11/pybind11.h>


namespace lh5 = lue::hdf5;
namespace ldm = lue::data_model;
using namespace pybind11::literals;


namespace lue::framework {
    namespace {

        template<typename Element, Rank rank>
        auto write_constant_array(
            [[maybe_unused]] PartitionedArray<Element, rank> const& array,
            [[maybe_unused]] std::string const& array_pathname) -> hpx::future<void>
        {
            auto const [dataset_pathname, phenomenon_name, property_set_name, layer_name] =
                parse_array_pathname(array_pathname);
            auto const [object_id, _, file_datatype] =
                ldm::constant::probe_raster(dataset_pathname, phenomenon_name, property_set_name, layer_name);

            verify_rank_supported(rank);

            using Policies = policy::write_into::DefaultValuePolicies<Element>;

            return write(Policies{}, array, array_pathname, object_id);
        }


        template<typename Element, Rank rank>
        auto write_variable_array(
            [[maybe_unused]] PartitionedArray<Element, rank> const& array,
            [[maybe_unused]] Index const time_step_idx,
            [[maybe_unused]] std::string const& array_pathname) -> hpx::future<void>
        {
            auto const [dataset_pathname, phenomenon_name, property_set_name, layer_name] =
                parse_array_pathname(array_pathname);
            auto const [object_id, _, file_datatype] =
                ldm::variable::probe_raster(dataset_pathname, phenomenon_name, property_set_name, layer_name);

            verify_rank_supported(rank);

            using Policies = policy::write_into::DefaultValuePolicies<Element>;

            return write(Policies{}, array, array_pathname, object_id, time_step_idx);
        }


        template<Arithmetic Element>
        void bind(pybind11::module& module)
        {
            Rank const rank{2};

            module.def("write_array", write_constant_array<Element, rank>);
            module.def("write_array", write_variable_array<Element, rank>);
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

    }  // Anonymous namespace


    void bind_write_array(pybind11::module& module)
    {
        bind<ArithmeticElements>(module);
    }

}  // namespace lue::framework

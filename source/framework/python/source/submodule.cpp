#include "lue/py/framework/submodule.hpp"
#include "hpx_runtime.hpp"
#include "lue/framework/algorithm/timestamp.hpp"
#include "lue/concept.hpp"
#include "lue/configure.hpp"
#include "lue/framework.hpp"
#include "lue/gdal.hpp"
#include <pybind11/numpy.h>
#include <pybind11/stl.h>


namespace lue::framework {
    namespace {

        HPXRuntime* runtime{nullptr};


        void start_hpx_runtime(std::vector<std::string> const& configuration)
        {
            // Iff the pointer to the runtime is not pointing to an instance,
            // instantiate one. This will start the HPX runtime.
            if (runtime == nullptr)
            {
                pybind11::gil_scoped_release release;
                runtime = new HPXRuntime{configuration};
            }
        }


        void stop_hpx_runtime()
        {
            // Iff the pointer to the runtime is pointing to an instance, delete
            // it. This will stop the HPX runtime.
            if (runtime != nullptr)
            {
                HPXRuntime* r = runtime;
                runtime = nullptr;
                pybind11::gil_scoped_release release;
                delete r;
            }
        }


        auto on_root_locality() -> bool
        {
            return hpx::find_here() == hpx::find_root_locality();
        }


        template<Arithmetic Element>
        auto type_of() -> pybind11::object
        {
            return pybind11::dtype::of<Element>().attr("type");
        }


        // TODO Find a way to do this without an Elements instance. We only have to iterate over the tuple's
        // types.

        template<typename... Elements>
        auto type_of([[maybe_unused]] std::tuple<Elements...>&& elements) -> std::vector<pybind11::object>
        {
            return {type_of<Elements>()...};
        }

    }  // Anonymous namespace


    void bind_hpx(pybind11::module& module);
    void bind_create_array(pybind11::module& module);
    void bind_hyperslab(pybind11::module& module);
    void bind_focal_operations(pybind11::module& module);
    void bind_global_operations(pybind11::module& module);
    void bind_local_operations(pybind11::module& module);
    void bind_miscellaneous_operations(pybind11::module& module);
    void bind_routing_operations(pybind11::module& module);
    void bind_zonal_operations(pybind11::module& module);
    void bind_partitioned_array(pybind11::module& module);
    void bind_scalar(pybind11::module& module);
    void bind_serial_route(pybind11::module& module);

#ifdef LUE_FRAMEWORK_WITH_DEVELOPMENT_OPERATIONS
    void bind_array_partition_id(pybind11::module& module);
    void bind_locality_id(pybind11::module& module);
    void bind_timestamp(pybind11::module& module);
#endif

    void bind_read_array(pybind11::module& module);
    void bind_write_array(pybind11::module& module);

    void bind_model(pybind11::module& module);
    void bind_progressor(pybind11::module& module);
    void bind_simulate(pybind11::module& module);

    void bind_from_numpy(pybind11::module& module);
    void bind_to_numpy(pybind11::module& module);

    void bind_from_gdal(pybind11::module& module);
    void bind_to_gdal(pybind11::module& module);

    void init_submodule(pybind11::module& module)
    {
        pybind11::module submodule = module.def_submodule(
            "framework",
            R"(
    :mod:`lue.framework` --- LUE Environmental Modelling Framework
    ==============================================================

    The :mod:`lue.framework` package ...
)");

        gdal::register_gdal_drivers();

        submodule.def("start_hpx_runtime", &start_hpx_runtime);

        submodule.def("stop_hpx_runtime", &stop_hpx_runtime);

        submodule.def("on_root_locality", &on_root_locality);

        bind_hpx(submodule);

        // Wrap configuration settings
        submodule.attr("arithmetic_element_types") = type_of(ArithmeticElements{});
        submodule.attr("unsigned_integral_element_types") = type_of(UnsignedIntegralElements{});
        submodule.attr("signed_integral_element_types") = type_of(SignedIntegralElements{});
        submodule.attr("integral_element_types") = type_of(IntegralElements{});
        submodule.attr("floating_point_element_types") = type_of(FloatingPointElements{});
        submodule.attr("signed_arithmetic_element_types") = type_of(SignedArithmeticElements{});
        submodule.attr("material_element_types") = type_of(MaterialElements{});
        submodule.attr("zone_element_types") = type_of(ZoneElements{});
        submodule.attr("boolean_element_type") = type_of<BooleanElement>();
        submodule.attr("count_element_type") = type_of<CountElement>();
        submodule.attr("id_element_type") = type_of<IDElement>();
        submodule.attr("index_element_type") = type_of<IndexElement>();
        submodule.attr("flow_direction_element_type") = type_of<FlowDirectionElement>();

        submodule.attr("with_development_operations") = BuildOptions::framework_with_development_operations;

        // Wrap high-level data structures
        bind_scalar(submodule);
        bind_partitioned_array(submodule);
        bind_serial_route(submodule);

        // Wrap high-level algorithms
        bind_create_array(submodule);
        bind_hyperslab(submodule);
        bind_focal_operations(submodule);
        bind_global_operations(submodule);
        bind_local_operations(submodule);
        bind_miscellaneous_operations(submodule);
        bind_routing_operations(submodule);
        bind_zonal_operations(submodule);
        bind_read_array(submodule);
        bind_write_array(submodule);

#ifdef LUE_FRAMEWORK_WITH_DEVELOPMENT_OPERATIONS
        bind_array_partition_id(submodule);

        if constexpr (lue::arithmetic_element_supported<std::uint32_t>)
        {
            bind_locality_id(submodule);
        }

        if constexpr (lue::arithmetic_element_supported<ClockTick>)
        {
            bind_timestamp(submodule);
        }
#endif

        bind_model(submodule);
        bind_progressor(submodule);
        bind_simulate(submodule);

        bind_from_numpy(submodule);
        bind_to_numpy(submodule);
        bind_from_gdal(submodule);
        bind_to_gdal(submodule);

        // Unless the user calls stop_hpx_runtime explicitly, we will do it
        // automatically upon module unload
        auto atexit = pybind11::module_::import("atexit");
        atexit.attr("register")(pybind11::cpp_function([]() { stop_hpx_runtime(); }));
    }

}  // namespace lue::framework

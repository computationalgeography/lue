#pragma once
#include <pybind11/pybind11.h>


namespace lue::data_model {

    void init_array(pybind11::module& module);

    void init_clock(pybind11::module& module);

    void init_dataset(pybind11::module& module);

    void init_epoch(pybind11::module& module);

    void init_active_object_id(pybind11::module& module);

    void init_active_object_index(pybind11::module& module);

    void init_active_set_index(pybind11::module& module);

    void init_location_in_time(pybind11::module& module);

    void init_mobile_space_box(pybind11::module& module);

    void init_mobile_space_point(pybind11::module& module);

    void init_object_id(pybind11::module& module);

    void init_object_tracker(pybind11::module& module);

    void init_validate(pybind11::module& module);

    void init_phenomenon(pybind11::module& module);

    void init_property_group(pybind11::module& module);

    void init_property_set(pybind11::module& module);

    void init_space_domain(pybind11::module& module);

    void init_stationary_space_box(pybind11::module& module);

    void init_stationary_space_point(pybind11::module& module);

    void init_time_box(pybind11::module& module);

    void init_time_cell(pybind11::module& module);

    void init_time_domain(pybind11::module& module);

    void init_time_point(pybind11::module& module);

    void init_universe(pybind11::module& module);

    void init_value_group(pybind11::module& module);

}  // namespace lue::data_model

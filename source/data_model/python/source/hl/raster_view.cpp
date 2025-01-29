#include "lue/data_model/hl/raster_view.hpp"
#include "submodule.hpp"
#include "lue/py/data_model/conversion.hpp"


using namespace pybind11::literals;


namespace lue::data_model {

    void bind_raster_view(pybind11::module& module)
    {
        using RasterViewBase = RasterView<Dataset*>;
        using ConstantRasterView = constant::RasterView<Dataset*>;
        using VariableRasterView = variable::RasterView<Dataset*>;

        pybind11::class_<RasterViewBase, DatasetView<Dataset*>>(module, "RasterViewBase");

        {
            pybind11::module submodule = module.def_submodule("constant");

            pybind11::class_<ConstantRasterView, RasterViewBase>(submodule, "RasterView")

                .def(
                    "add_layer",
                    [](ConstantRasterView& self, std::string const& name, pybind11::object const& dtype_args)
                    {
                        pybind11::dtype const dtype{pybind11::dtype::from_args(dtype_args)};
                        hdf5::Datatype const datatype = numpy_type_to_memory_datatype(dtype);

                        return self.add_layer(name, datatype);
                    });
        }


        module.def(
            "create_raster_view",
            [](Dataset& dataset,
               std::string const& phenomenon_name,
               std::string const& property_set_name,
               pybind11::tuple const& grid_shape,
               typename RasterViewBase::SpaceBox const& space_box)

            {
                auto const grid_shape_ = tuple_to_shape(grid_shape);

                return constant::create_raster_view<Dataset*>(
                    &dataset, phenomenon_name, property_set_name, grid_shape_, space_box);
            },
            // Keep the dataset (arg 1) alive as long as the view (arg 0)
            // is alive
            pybind11::keep_alive<0, 1>());


        {
            pybind11::module submodule = module.def_submodule("variable");

            pybind11::class_<VariableRasterView, RasterViewBase>(submodule, "RasterView")

                .def(
                    "add_layer",
                    [](VariableRasterView& self, std::string const& name, pybind11::object const& dtype_args)
                    {
                        pybind11::dtype const dtype{pybind11::dtype::from_args(dtype_args)};
                        hdf5::Datatype const datatype = numpy_type_to_memory_datatype(dtype);

                        return self.add_layer(name, datatype);
                    });
        }


        module.def(
            "create_raster_view",
            [](Dataset& dataset,
               std::string const& phenomenon_name,
               std::string const& property_set_name,
               Clock const& clock,
               Count nr_time_steps,
               typename VariableRasterView::TimeBox const& time_box,
               pybind11::tuple const& grid_shape,
               typename RasterViewBase::SpaceBox const& space_box)
            {
                auto const grid_shape_ = tuple_to_shape(grid_shape);

                return variable::create_raster_view<Dataset*>(
                    &dataset,
                    phenomenon_name,
                    property_set_name,
                    clock,
                    nr_time_steps,
                    time_box,
                    grid_shape_,
                    space_box);
            },
            // Keep the dataset (arg 1) alive as long as the view (arg 0)
            // is alive
            pybind11::keep_alive<0, 1>());
    }

}  // namespace lue::data_model

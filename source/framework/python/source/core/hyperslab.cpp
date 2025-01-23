#include "lue/framework/core/hyperslab.hpp"
#include <pybind11/stl.h>
#include <tuple>


using namespace pybind11::literals;


namespace lue::framework {
    namespace {

        auto to_center(std::tuple<Index, Index> const& center)
        {
            return Hyperslab<2>::Center{std::get<0>(center), std::get<1>(center)};
        };

        auto to_shape(std::tuple<Index, Index> const& shape)
        {
            return Hyperslab<2>::Shape{std::get<0>(shape), std::get<1>(shape)};
        };

    }  // Anonymous namespace


    void bind_hyperslab(pybind11::module& module)
    {
        Rank const rank{2};
        using Hyperslab = lue::Hyperslab<rank>;

        pybind11::class_<Hyperslab>(module, "Hyperslab")
            .def(
                pybind11::init(
                    [](std::tuple<Index, Index> const& center, std::tuple<Index, Index> const& shape)
                    { return Hyperslab{to_center(center), to_shape(shape)}; }),
                R"(
    Construct a hyperslab given a @a center cell and a @a shape

    :param center: Indices of center cell
    :param shape: Number of cells in each dimension
)",
                "center"_a,
                "shape"_a)
            .def_property_readonly("offsets", &Hyperslab::offsets)
            .def_property_readonly("counts", &Hyperslab::counts)
            .def_property_readonly("strides", &Hyperslab::strides);
    }

}  // namespace lue::framework

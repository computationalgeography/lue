#include "lue/framework/core/component/partitioned_array.hpp"
#include "lue/py/data_model/numpy.hpp"
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <pybind11/pybind11.h>


DEFINE_INIT_NUMPY()


namespace py = pybind11;


namespace lue {
namespace framework {

template<
    typename T,
    Rank rank>
PartitionedArray<T, rank> create_array(
    py::array_t<T, py::array::c_style | py::array::forcecast> const& elements,
    ShapeT<PartitionedArray<T, rank>> const& max_partition_shape)
{
    using Shape = ShapeT<PartitionedArray<T, rank>>;

    py::buffer_info const array_info{elements.request()};

    if(array_info.ndim != rank) {
        throw std::runtime_error("Rank of array must equal rank of partition shape");
    }

    std::vector<ssize_t> const& shape1{array_info.shape};
    Shape shape{};
    std::copy(shape1.begin(), shape1.end(), shape.begin());

    return PartitionedArray<T, rank>{shape, /* elements, */ max_partition_shape};
}


PartitionedArray<double, 2> create_array(
    py::array_t<double, py::array::c_style | py::array::forcecast> const& elements,
    ShapeT<PartitionedArray<double, 2>> const& max_partition_shape)
{
    return create_array<double, 2>(elements, max_partition_shape);
}


void init_partitioned_array(
    py::module& module)
{
    init_numpy();

    py::class_<
            PartitionedArray<double, 2>>(
        module,
        "PartitionedArray",
        "PartitionedArray docstring...")
        ;

    module.def(
        "create_array",
        [](
            py::array_t<double, py::array::c_style | py::array::forcecast> const& elements,
            ShapeT<PartitionedArray<double, 2>> const& max_partition_shape)
        {
            return create_array(elements, max_partition_shape);
        },
        py::return_value_policy::move);
}

}  // namespace framework
}  // namespace lue

#include "submodule.hpp"
#include <pybind11/numpy.h>
#include <lue/core/define.hpp>
#include <lue/test/algorithm.hpp>
#include <nonstd/span.hpp>


namespace py = pybind11;


namespace lue {
    namespace data_model {
        namespace test {

            void init_submodule(py::module& module)
            {
                py::module submodule = module.def_submodule("test");

                submodule.def(
                    "select_random_ids",
                    [](py::array_t<Count> const& active_set_sizes,
                       py::array_t<Index>& active_set_idxs,
                       py::array_t<ID>& active_ids,
                       Count const max_nr_objects)
                    {
                        nonstd::span<Count> const active_set_sizes_{
                            const_cast<py::array_t<Count>&>(active_set_sizes).mutable_data(),
                            static_cast<nonstd::span<Count>::size_type>(active_set_sizes.size())};
                        nonstd::span<Index> active_set_idxs_{
                            active_set_idxs.mutable_data(),
                            static_cast<nonstd::span<Index>::size_type>(active_set_idxs.size())};
                        nonstd::span<ID> active_ids_{
                            active_ids.mutable_data(),
                            static_cast<nonstd::span<ID>::size_type>(active_ids.size())};

                        select_random_ids(active_set_sizes_, active_set_idxs_, active_ids_, max_nr_objects);
                    });

                submodule.def(
                    "select_random_ids",
                    [](py::array_t<ID>& active_ids, Count const max_nr_objects)
                    {
                        nonstd::span<ID> active_ids_{
                            active_ids.mutable_data(),
                            static_cast<nonstd::span<ID>::size_type>(active_ids.size())};

                        select_random_ids(active_ids_, max_nr_objects);
                    });
            }

        }  // namespace test
    }      // namespace data_model
}  // namespace lue

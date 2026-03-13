#include "lue/framework/algorithm/value_policies/open_simplex_noise.hpp"
#include "lue/framework.hpp"
#include "lue/py/bind.hpp"
#include <pybind11/pybind11.h>


using namespace pybind11::literals;


namespace lue::framework {
    namespace {

        class Binder
        {

            public:

                template<std::floating_point Element>
                static void bind(pybind11::module& module)
                {
                    Rank const rank{2};

                    module.def(
                        "open_simplex_noise",
                        [](PartitionedArray<Element, rank> const& x_coordinates,
                           PartitionedArray<Element, rank> const& y_coordinates,
                           int const seed) -> auto
                        { return value_policies::open_simplex_noise(x_coordinates, y_coordinates, seed); },
                        "x_coordinates"_a,
                        "y_coordinates"_a,
                        "seed"_a);
                }
        };

    }  // Anonymous namespace


    void bind_open_simplex_noise(pybind11::module& module)
    {
        bind<Binder, FloatingPointElements>(module);
    }

}  // namespace lue::framework

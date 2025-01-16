#include "lue/framework/configure.hpp"
#include "lue/framework/core/component.hpp"
#include "lue/framework/partitioned_array.hpp"
#include "lue/framework.hpp"
#include "lue/py/bind.hpp"


namespace lue::framework {
    namespace {

        class Binder
        {

            public:

                template<Arithmetic Element>
                static void bind(pybind11::module& module)
                {
                    Rank const rank{2};

                    module.def(
                        "wait",
                        [](PartitionedArray<Element, rank> const& array)
                        {
                            wait_all(array);

                            lue_hpx_assert(all_are_valid(array));
                            lue_hpx_assert(all_are_ready(array));
                        });
                }
        };

    }  // Anonymous namespace


    void bind_wait_partitioned_array(pybind11::module& module)
    {
        bind<Binder, ArithmeticElements>(module);
    }

}  // namespace lue::framework

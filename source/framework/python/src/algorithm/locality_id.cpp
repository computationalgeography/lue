#include "lue/framework/algorithm/locality_id.hpp"
#include "lue/bind.hpp"
#include "lue/framework.hpp"


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
                        "locality_id",
                        [](PartitionedArray<Element, rank> const& array) { return locality_id(array); });
                }
        };

    }  // Anonymous namespace


    void bind_locality_id(pybind11::module& module)
    {
        static_assert(
            lue::arithmetic_element_supported<std::uint32_t>,
            "Reconfigure: locality IDs are represented by std::uint32_t elements "
            "but LUE is built without support for that type");

        bind<Binder, ArithmeticElements>(module);
    }

}  // namespace lue::framework

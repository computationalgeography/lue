#include "lue/framework/algorithm/iterate_per_element.hpp"
#include "lue/framework.hpp"
#include "lue/py/bind.hpp"


namespace lue::framework {
    namespace {

        template<typename Element, Rank rank>
        auto iterate_per_element(PartitionedArray<Element, rank> const& array)
            -> PartitionedArray<Element, rank>
        {
            using Policies = policy::iterate_per_element::DefaultPolicies<Element>;

            return iterate_per_element(Policies{}, array);
        }


        class Binder
        {

            public:

                template<Arithmetic Element>
                static void bind(pybind11::module& module)
                {
                    Rank const rank{2};

                    module.def("iterate_per_element", iterate_per_element<Element, rank>);
                }
        };

    }  // Anonymous namespace


    void bind_iterate_per_element(pybind11::module& module)
    {
        bind<Binder, ArithmeticElements>(module);
    }

}  // namespace lue::framework

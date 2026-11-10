#include "stress.hpp"
#include "lue/framework/api/cxx/as_field.hpp"
#include "lue/framework/api/cxx/as_state.hpp"
#include "lue/framework/api/cxx/local_operation.hpp"
#include "lue/framework/api/cxx/miscellaneous_operation.hpp"
#include "lue/framework/model.hpp"
#include "lue/framework.hpp"
#include <hpx/hpx_main.hpp>


namespace {

    class Model: public lue::Model
    {

        public:

            static lue::Rank const rank{2};

            using Shape = lue::Shape<lue::Count, rank>;

            using FromElement = lue::LargestUnsignedIntegralElement;

            using ToElement = FromElement;

            using LookupTable = lue::LookupTable<FromElement, ToElement>;


            Model(Shape const array_shape, Shape const partition_shape):
                lue::Model{},
                _array_shape{array_shape},
                _partition_shape{partition_shape}

            {
            }


            void initialize() final
            {
                using namespace lue::api;

                _field = uniform(
                    _array_shape,
                    _partition_shape,
                    as_field(create_scalar(Literal{FromElement{1}})),
                    as_field(create_scalar(Literal{FromElement{5}})));

                LookupTable lookup_table{
                    {1, 1},
                    {2, 2},
                    {3, 3},
                    {4, 4},
                    {5, 5},
                };

                _lookup_table = lookup_table;
            }


            auto simulate([[maybe_unused]] lue::Count const time_step) -> hpx::shared_future<void> final
            {
                using namespace lue::api;

                _field = reclassify(_field, _lookup_table);

                return as_state(_field);
            }


        private:

            Shape _array_shape;

            Shape _partition_shape;

            lue::api::Field _field;

            lue::api::LookupTable _lookup_table;
    };

}  // Anonymous namespace


auto main(int argc, char* argv[]) -> int
{
    return lue::stress::run_stress_test<Model>(argc, argv);
}

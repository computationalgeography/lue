#include "lue/framework/api/cxx/focal_operation.hpp"
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

            using FloatElement = lue::FloatingPointElement<0>;


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
                    as_field(create_scalar(Literal{FloatElement{0}})),
                    as_field(create_scalar(Literal{FloatElement{1}})));
            }


            auto simulate([[maybe_unused]] lue::Count const time_step) -> hpx::shared_future<void> final
            {
                using namespace lue::api;

                auto const kernel = lue::box_kernel<lue::BooleanElement, 2>(1, 1);

                _field = focal_mean(focal_sum(_field, kernel), kernel);

                return as_state(_field);
            }


        private:

            Shape _array_shape;

            Shape _partition_shape;

            lue::api::Field _field;
    };

}  // Anonymous namespace


auto main(int argc, char* argv[]) -> int
{
    return lue::stress::run_stress_test<Model>(argc, argv);
}

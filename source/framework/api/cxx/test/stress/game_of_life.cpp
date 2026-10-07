#include "stress.hpp"
#include "lue/framework/api/cxx/as_field.hpp"
#include "lue/framework/api/cxx/as_state.hpp"
#include "lue/framework/api/cxx/focal_operation.hpp"
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

            using DeadOrAlive = lue::SmallestUnsignedIntegralElement;
            using NrNeighbours = lue::SmallestUnsignedIntegralElement;
            using Generation = lue::PartitionedArray<DeadOrAlive, rank>;

            using Kernel = lue::Kernel<lue::BooleanElement, rank>;


            Model(Shape const array_shape, Shape const partition_shape):
                lue::Model{},
                _array_shape{array_shape},
                _partition_shape{partition_shape}

            {
            }


            void initialize() final
            {
                using namespace lue::api;

                FloatElement const fraction_alive_cells = 0.25;
                auto const random_field = uniform(
                    _array_shape,
                    _partition_shape,
                    as_field(create_scalar(Literal{FloatElement{0}})),
                    as_field(create_scalar(Literal{FloatElement{1}})));

                // This assumes std::is_same_v<DeadOrAlive, lue::BooleanElement> is true. If not (this depends
                // on how LUE is configured at build-time), then cast the array like this:
                // cast<DeadOrAlive>(...);
                _generation = random_field <= fraction_alive_cells;
            }


            auto simulate([[maybe_unused]] lue::Count const time_step) -> hpx::shared_future<void> final
            {
                using namespace lue::api;

                Shape shape{3, 3};
                Kernel kernel{
                    shape,
                    {
                        // clang-format off
                        1, 1, 1,
                        1, 0, 1,
                        1, 1, 1,
                        // clang-format on
                    }};

                auto nr_alive_cells = focal_sum(_generation, kernel);

                // Next state of currently alive cells
                auto underpopulated = nr_alive_cells < NrNeighbours{2};
                auto overpopulated = nr_alive_cells > NrNeighbours{3};

                // Next state of currently dead cells
                auto reproducing = nr_alive_cells == NrNeighbours{3};

                _generation = where(
                    _generation,
                    // True if alive and not under/overpopulated
                    !(underpopulated || overpopulated),
                    // True if dead with three neighbours
                    reproducing);

                return as_state(_generation);
            }


        private:

            Shape _array_shape;

            Shape _partition_shape;

            lue::api::Field _generation;
    };

}  // Anonymous namespace


auto main(int argc, char* argv[]) -> int
{
    // TODO: Does this model hang sometimes? If so, what is special about GoL compared to local operations and
    // focal operations?
    return lue::stress::run_stress_test<Model>(argc, argv);
}

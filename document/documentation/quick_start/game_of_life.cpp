#include "lue/framework/algorithm/kernel.hpp"
#include "lue/framework/algorithm/value_policies/comparison.hpp"
#include "lue/framework/algorithm/value_policies/focal_sum.hpp"
#include "lue/framework/algorithm/value_policies/logical.hpp"
#include "lue/framework/algorithm/value_policies/uniform.hpp"
#include "lue/framework/algorithm/value_policies/where.hpp"
#include "lue/framework/io.hpp"
#include "lue/framework/model.hpp"
#include "lue/framework.hpp"
#include "lue/gdal.hpp"
#include <cxxopts.hpp>
#include <hpx/hpx_main.hpp>
#include <cstdlib>
#include <filesystem>
#include <format>
#include <optional>
#include <ranges>
#include <tuple>
#include <utility>


using Count = lue::Count;
using Rank = lue::Rank;
Rank const rank{2};
using Shape = lue::Shape<Count, rank>;

using DeadOrAlive = lue::SmallestUnsignedIntegralElement;
using NrNeighbours = lue::SmallestUnsignedIntegralElement;
using Generation = lue::PartitionedArray<DeadOrAlive, rank>;

using Kernel = lue::Kernel<lue::BooleanElement, rank>;

using namespace lue::value_policies;


auto initialize_generation(Shape const& array_shape, std::optional<Shape> const& partition_shape)
    -> Generation
{
    float const fraction_alive_cells = 0.25;
    auto const random_field = partition_shape ? uniform<float>(array_shape, *partition_shape, 0.F, 1.F)
                                              : uniform<float>(array_shape, 0.F, 1.F);
    auto generation = random_field <= fraction_alive_cells;

    // This assumes std::is_same_v<DeadOrAlive, lue::BooleanElement> is true. If not (this depends on how LUE
    // is configured at build-time), then cast the array like this:
    // return cast<DeadOrAlive>(generation);
    return generation;
}


auto next_generation(Generation const& generation) -> Generation
{
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

    auto nr_alive_cells = focal_sum(generation, kernel);

    // Next state of currently alive cells
    auto underpopulated = nr_alive_cells < NrNeighbours{2};
    auto overpopulated = nr_alive_cells > NrNeighbours{3};

    // Next state of currently dead cells
    auto reproducing = nr_alive_cells == NrNeighbours{3};

    auto next_generation = where(
        generation,
        // True if alive and not under/overpopulated
        !(underpopulated || overpopulated),
        // True if dead with three neighbours
        reproducing);

    return next_generation;
}


class GameOfLife: public lue::Model
{
    public:

        GameOfLife(
            Shape const& array_shape,
            std::optional<Shape> const& partition_shape,
            std::filesystem::path generation_path):

            lue::Model{},
            _array_shape{array_shape},
            _partition_shape{partition_shape},
            _generation_path{std::move(generation_path)}

        {
        }


        void initialize() final
        {
            _generation = initialize_generation(_array_shape, _partition_shape);
        }


        auto simulate([[maybe_unused]] lue::Count const iteration) -> hpx::shared_future<void> final
        {
            _generation = next_generation(_generation);
            save_generation(_generation, iteration);

            return hpx::when_all(_generation.partitions().begin(), _generation.partitions().end()).share();
        }


    private:

        void save_generation(Generation const& generation, Count const generation_id)
        {
            lue::to_gdal(generation, std::format("{}-{}.tif", _generation_path.string(), generation_id));
        }

        Generation _generation;

        Shape _array_shape;

        std::optional<Shape> _partition_shape;

        std::filesystem::path _generation_path;
};


void game_of_life(
    Shape const& array_shape,
    std::optional<Shape> const& partition_shape,
    Count const nr_generations,
    std::filesystem::path const& generation_path)
{
    auto model = GameOfLife(array_shape, partition_shape, generation_path);

    lue::Progressor progressor{};

    lue::run_deterministic(model, progressor, nr_generations, 2);
}


void handle_error(std::string const& message)
{
    std::cerr << std::format("{}\n", message);
    exit(EXIT_FAILURE);
}


void handle_info(std::string const& message)
{
    std::cout << std::format("{}\n", message);
    exit(EXIT_SUCCESS);
}


auto parse_shape(std::string const& shape_as_string) -> Shape
{
    std::vector<Count> extents{};

    try
    {
        auto extents_view = shape_as_string | std::views::split(',') |
                            std::views::transform(
                                [](auto const& subrange) -> Count
                                { return std::stoull(std::string(subrange.begin(), subrange.end())); });
        // | std::ranges::to<std::vector>();  // Bug in Clang prevents the use of ranges::to for now

        for (auto view : extents_view)
        {
            extents.push_back(view);
        }
    }
    catch (std::exception const& exception)
    {
        handle_error(std::format(
            "A shape must be formatted as a list of {} sizes, separated by a comma (got: {})",
            rank,
            shape_as_string));
    }

    if (std::size(extents) != rank)
    {
        handle_error(std::format(
            "A shape must be formatted as a list of {} sizes, separated by a comma (got: {})",
            rank,
            shape_as_string));
    }

    return {extents[0], extents[1]};
}


auto parse_command_line(std::span<char*> const& argv)
    -> std::tuple<Shape, std::optional<Shape>, Count, std::filesystem::path>
{
    cxxopts::Options options{
        std::filesystem::path{argv[0]}.filename(),
        "Calculate the generations of alive cells according to the Game of Life cellular automaton"};

    options.positional_help("<array_shape> <nr_generations> <pathname>");
    options.add_options()
        // clang-format off
        ("array_shape", "Shape of the array", cxxopts::value<std::string>())
        ("partition_shape", "Shape of the array partitions. If not provided, a default shape will be used.",
            cxxopts::value<std::string>())
        ("nr_generations", "Number of Game of Life generations to calculate", cxxopts::value<Count>())
        ("pathname", "Pathname of GeoTiffs, without extension", cxxopts::value<std::string>())
        ("h,help", "Show usage")
        // clang-format on
        ;
    options.parse_positional({"array_shape", "nr_generations", "pathname"});

    cxxopts::ParseResult parse_result{};

    parse_result = options.parse(static_cast<int>(argv.size()), argv.data());

    if (parse_result.count("help") != 0)
    {
        handle_info(options.help());
    }

    if (!parse_result.unmatched().empty())
    {
        handle_error(options.help());
    }

    Shape array_shape{parse_shape(parse_result["array_shape"].as<std::string>())};

    std::optional<Shape> partition_shape{};

    if (parse_result.count("partition_shape") > 0)
    {
        partition_shape = parse_shape(parse_result["partition_shape"].as<std::string>());
    }

    Count nr_generations{parse_result["nr_generations"].as<Count>()};
    std::filesystem::path path{parse_result["pathname"].as<std::string>()};

    return {array_shape, partition_shape, nr_generations, path};
}


auto main(int argc, char* argv[]) -> int
{
    int status = EXIT_FAILURE;

    try
    {
        auto const [array_shape, partition_shape, nr_generations, path] =
            parse_command_line(std::span(argv, argc));

        lue::gdal::register_gdal_drivers();

        game_of_life(array_shape, partition_shape, nr_generations, path);

        status = EXIT_SUCCESS;
    }
    catch (std::exception const& exception)
    {
        handle_error(exception.what());
    }

    return status;
}

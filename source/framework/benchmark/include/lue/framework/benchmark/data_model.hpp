#pragma once
#include "lue/data_model/hl.hpp"


namespace lue::benchmark {

    template<typename Count, Rank rank>
    hdf5::Shape shape_to_shape(lue::Shape<Count, rank> const& from_shape)
    {
        hdf5::Shape to_shape(rank);

        for (std::size_t i = 0; i < rank; ++i)
        {
            to_shape[i] = from_shape[i];
        }

        return to_shape;
    }


    template<typename Count, Rank rank>
    std::tuple<data_model::ID, hdf5::Hyperslab> array_info(
        std::string const& array_pathname,
        std::vector<Index> center_cell,
        Shape<Count, rank> const& array_shape)
    {
        using Dataset = data_model::Dataset;
        using RasterView = data_model::constant::RasterView<Dataset*>;

        auto const [dataset_pathname, phenomenon_name, property_set_name, layer_name] =
            parse_array_pathname(array_pathname);

        Dataset dataset{dataset_pathname};
        RasterView raster_view{&dataset, phenomenon_name, property_set_name};
        data_model::ID const object_id{raster_view.object_id()};

        if (center_cell.empty())
        {
            // Default center cell is the center cell of the array
            hdf5::Shape const& grid_shape{raster_view.grid_shape()};

            center_cell.push_back(grid_shape[0] / 2);
            center_cell.push_back(grid_shape[1] / 2);
        }

        lue_hpx_assert(center_cell.size() == 2);

        hdf5::Shape const h5_array_shape{shape_to_shape(array_shape)};

        hdf5::Offset start{
            center_cell[0] - (h5_array_shape[0] / 2), center_cell[1] - (h5_array_shape[1] / 2)};
        hdf5::Count count{h5_array_shape[0], h5_array_shape[1]};

        hdf5::Hyperslab hyperslab{start, count};

        lue_hpx_assert(hyperslab.nr_dimensions() == 2);
        lue_hpx_assert(static_cast<Count>(hyperslab.count()[0]) == array_shape[0]);
        lue_hpx_assert(static_cast<Count>(hyperslab.count()[1]) == array_shape[1]);

        return std::make_tuple(object_id, std::move(hyperslab));
    }

}  // namespace lue::benchmark

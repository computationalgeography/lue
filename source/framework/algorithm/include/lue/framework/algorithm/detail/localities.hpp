#pragma once
#include "lue/framework/core/array.hpp"


namespace lue::detail {

    template<typename Component, Rank rank>
    auto localities(Array<Component, rank> const& components) -> Array<hpx::future<hpx::id_type>, rank>
    {
        Array<hpx::future<hpx::id_type>, rank> locality_fs{components.shape()};
        Count const nr_components{nr_elements(components.shape())};

        for (Index idx = 0; idx < nr_components; ++idx)
        {
            locality_fs[idx] = hpx::dataflow(
                hpx::launch::async,
                hpx::unwrapping(
                    [](hpx::id_type const& component_id)
                    {
                        hpx::future<hpx::id_type> locality_f{hpx::get_colocation_id(component_id)};

                        return locality_f;
                    }),
                components[idx]);
        }

        return locality_fs;
    }

}  // namespace lue::detail

#pragma once
#include "lue/framework/core/array.hpp"
#include <hpx/include/async.hpp>


namespace lue::detail {

    using IDPromise = hpx::lcos::local::promise<hpx::id_type>;

    template<
        Rank rank>
    using IDPromiseArray = Array<IDPromise, rank>;

}  // namespace lue::detail

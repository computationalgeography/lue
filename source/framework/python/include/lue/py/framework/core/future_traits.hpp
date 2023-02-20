#pragma once
#include "lue/framework/core/type_traits.hpp"


namespace lue::detail {

    template<typename E>
    class ArrayTraits<hpx::shared_future<E>>
    {

        public:

            using Element = E;
    };

}  // namespace lue::detail

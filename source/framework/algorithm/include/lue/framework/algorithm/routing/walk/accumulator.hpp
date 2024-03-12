#pragma once
#include "lue/framework/core/define.hpp"
#include <hpx/serialization.hpp>
#include <map>


namespace lue {

    template<typename RouteID, typename Element>
    class Accumulator
    {

        public:

            Accumulator() = default;

            auto value(RouteID const route_id) -> Element&
            {
                // A zero-initialized value is inserted if no value for the route_id is
                // present in the map
                return _values[route_id];
            }

        protected:

            friend class hpx::serialization::access;

            template<typename Archive>
            void serialize(Archive& archive, [[maybe_unused]] unsigned int const version)
            {
                // clang-format off
                archive & _values;
                // clang-format on
            }

        private:

            std::map<RouteID, Element> _values{};
    };

}  // namespace lue

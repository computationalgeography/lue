#pragma once
#include "lue/framework/partitioned_route/serial_route_fragment.hpp"
#include "lue/stream.hpp"


namespace lue {

    template<Rank rank>
    std::ostream& operator<<(std::ostream& stream, SerialRouteFragment<rank> const& fragment)
    {
        stream << "cell_idxs             : " << fragment.cells_idxs() << "\n"
               << "next_fragment_location: " << fragment.next_fragment_location() << "\n";

        return stream;
    }

}  // namespace lue

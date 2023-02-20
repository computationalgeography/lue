#pragma once
#include <array>
#include <string>


namespace lue {
    namespace hl {

        class RasterDomain
        {

            public:

                using Coordinate = double;
                using Coordinates = std::array<Coordinate, 4>;

                RasterDomain();

                RasterDomain(RasterDomain const&) = default;

                RasterDomain(RasterDomain&&) = default;

                RasterDomain(
                    std::string const& crs,
                    Coordinate west,
                    Coordinate south,
                    Coordinate east,
                    Coordinate north);

                RasterDomain(std::string const& crs, Coordinates&& coordinates);

                ~RasterDomain() = default;

                RasterDomain& operator=(RasterDomain const&) = default;

                RasterDomain& operator=(RasterDomain&&) = default;

                bool operator==(RasterDomain const& other) const;

                bool operator<(RasterDomain const& other) const;

                std::string const& crs() const;

                Coordinate west() const;

                Coordinate south() const;

                Coordinate east() const;

                Coordinate north() const;

                Coordinates const& coordinates() const;

            private:

                std::string _crs;

                Coordinates _coordinates;
        };

    }  // namespace hl
}  // namespace lue

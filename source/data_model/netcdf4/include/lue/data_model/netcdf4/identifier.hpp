#pragma once
#include <vector>


namespace lue::data_model::netcdf4 {

    class Identifier
    {

        public:

            Identifier();

            Identifier(int const id);

            ~Identifier()=default;

            operator int() const;

            bool is_valid() const;

            void invalidate();

        private:

            int _id;

    };


    using Identifiers = std::vector<Identifier>;

}  // namespace lue::data_model::netcdf4

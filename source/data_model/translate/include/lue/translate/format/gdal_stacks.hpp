#pragma once
#include "lue/hl/raster_domain.hpp"
#include "lue/hl/raster_stack_discretization.hpp"
#include "lue/translate/format/gdal_stack.hpp"
#include <vector>


namespace lue {
    namespace utility {

        /*!
            @brief      This class models a collection of raster stacks with the same
                        domain and discretization
        */
        class GDALStacks
        {

            public:

                using Collection = std::vector<GDALStack>;
                using Iterator = Collection::iterator;
                using ConstIterator = Collection::const_iterator;

                explicit GDALStacks(std::vector<std::string> const& dataset_names);

                GDALStacks(GDALStacks const& other) = default;

                GDALStacks(GDALStacks&& other) = default;

                ~GDALStacks() = default;

                GDALStacks& operator=(GDALStacks const& other) = default;

                GDALStacks& operator=(GDALStacks&& other) = default;

                hl::RasterDomain domain() const;

                hl::RasterStackDiscretization discretization() const;

                std::size_t size() const;

                Iterator begin();

                Iterator end();

                ConstIterator begin() const;

                ConstIterator end() const;

            private:

                std::vector<GDALStack> _stacks;

                hl::RasterDomain _domain;

                hl::RasterStackDiscretization _discretization;
        };

    }  // namespace utility
}  // namespace lue

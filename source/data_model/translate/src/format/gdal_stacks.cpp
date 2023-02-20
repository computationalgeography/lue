#include "lue/translate/format/gdal_stacks.hpp"
#include <cassert>
#include <set>


namespace lue {
    namespace utility {

        GDALStacks::GDALStacks(std::vector<std::string> const& dataset_names)

            :
            _stacks{},
            _domain{},
            _discretization{}

        {
            _stacks.reserve(dataset_names.size());

            for (auto const& dataset_name : dataset_names)
            {
                _stacks.emplace_back(dataset_name);
            }

            std::set<hl::RasterDomain> domains;
            std::set<hl::RasterStackDiscretization> discretizations;

            for (auto& stack : _stacks)
            {
                domains.emplace(stack.domain());
                discretizations.emplace(stack.discretization());
            }

            if (!_stacks.empty())
            {
                if (domains.size() != 1)
                {
                    throw std::runtime_error("Domains of raster stacks must be equal, but they are not");
                }

                if (discretizations.size() != 1)
                {
                    throw std::runtime_error("Discretizations of raster stacks must be equal, but they are "
                                             "not");
                }

                _domain = *domains.begin();
                _discretization = *discretizations.begin();
            }
        }


        hl::RasterDomain GDALStacks::domain() const
        {
            return _domain;
        }


        hl::RasterStackDiscretization GDALStacks::discretization() const
        {
            return _discretization;
        }


        std::size_t GDALStacks::size() const
        {
            return _stacks.size();
        }


        GDALStacks::Collection::iterator GDALStacks::begin()
        {
            return _stacks.begin();
        }


        GDALStacks::Collection::iterator GDALStacks::end()
        {
            return _stacks.end();
        }


        GDALStacks::Collection::const_iterator GDALStacks::begin() const
        {
            return _stacks.begin();
        }


        GDALStacks::Collection::const_iterator GDALStacks::end() const
        {
            return _stacks.end();
        }

    }  // namespace utility
}  // namespace lue

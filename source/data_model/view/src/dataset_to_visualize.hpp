#pragma once
#include "lue/object/dataset.hpp"
#include <boost/filesystem.hpp>
#include <optional>


namespace lue {
namespace utility {

class DatasetToVisualize
{

public:

                   DatasetToVisualize  (std::string const& name);

    std::string const& name            () const;

    Dataset const& dataset             () const;

    boost::filesystem::path const& path() const;

    std::string    pathname            () const;

    std::string    filename            () const;

    bool           is_open             () const;

    void           rescan              ();

private:

    bool           exists              () const;

    bool           changed             () const;

    std::string const _name;

    std::optional<Dataset> _dataset;

    boost::filesystem::path _path;

    std::time_t    _write_time;

};

}  // namespace utility
}  // namespace lue

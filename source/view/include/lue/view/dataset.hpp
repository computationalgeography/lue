#pragma once
#include "lue/object/dataset.hpp"
#include <boost/filesystem.hpp>
#include <optional>


namespace lue {
namespace view {

class Dataset
{

public:

                   Dataset             (std::string const& name);

    std::string const& name            () const;

    data_model::Dataset const&
                   dataset             () const;

    boost::filesystem::path const& path() const;

    std::string    pathname            () const;

    std::string    filename            () const;

    bool           is_open             () const;

    void           rescan              ();

private:

    bool           exists              () const;

    bool           changed             () const;

    //! Name of dataset as passed into constructor
    std::string const _name;

    //! Dataset instance, if opened
    std::optional<data_model::Dataset> _dataset;

    //! Canonical pathname of dataset
    boost::filesystem::path _path;

    //! Last time the dataset was updated
    std::time_t    _write_time;

};

}  // namespace view
}  // namespace lue

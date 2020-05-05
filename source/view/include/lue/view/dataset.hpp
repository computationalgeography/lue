#pragma once
// #include "lue/view/cache.hpp"
#include "lue/object/dataset.hpp"
#include <boost/filesystem.hpp>
#include <memory>
#include <optional>


namespace lue {
namespace view {

class Dataset
{

public:

    // using CachePtr = std::shared_ptr<Cache>;

    explicit       Dataset             (std::string const& name);

    std::string const& name            () const;

    data_model::Dataset const&
                   dataset             () const;

    // CachePtr       cache               ();

    boost::filesystem::path const& path() const;

    std::time_t    write_time          () const;

    std::string    pathname            () const;

    std::string    filename            () const;

    bool           is_open             () const;

    bool           rescan              ();

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

    // CachePtr       _cache;

};

}  // namespace view
}  // namespace lue

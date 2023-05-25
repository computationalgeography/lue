#include "lue/view/dataset.hpp"


namespace lue {
    namespace view {

        /*!
            @brief      Construct an instance
            @param      name Name of dataset

            A dataset named @a name must exist.
        */
        Dataset::Dataset(std::string const& name):

            _name{name},
            _dataset{std::make_optional<data_model::Dataset>(name)},
            _path{std::filesystem::canonical(_dataset->pathname())},
            _write_time{std::filesystem::last_write_time(_path)}
        // _cache{std::make_shared<Cache>()}

        {
        }


        /*!
            @brief      Return the name passed into the constructor
        */
        std::string const& Dataset::name() const
        {
            return _name;
        }


        /*!
            @brief      Return the layered dataset
            @warning    A dataset must be opened
        */
        data_model::Dataset const& Dataset::dataset() const
        {
            assert(_dataset);
            return *_dataset;
        }


        // Dataset::CachePtr Dataset::cache()
        // {
        //     return _cache;
        // }


        /*!
            @brief      Return the normalized pathname
        */
        std::filesystem::path const& Dataset::path() const
        {
            return _path;
        }


        std::filesystem::file_time_type Dataset::write_time() const
        {
            return _write_time;
        }


        /*!
            @brief      Return the normalized pathname, as a string
        */
        std::string Dataset::pathname() const
        {
            return _path.string();
        }


        std::string Dataset::filename() const
        {
            return _path.filename().string();
        }


        /*!
            @brief      Handle removal, reappearance or update of dataset
            @return     Whether or not the dataset changed since the previous
                        time it was opened
        */
        bool Dataset::rescan()
        {
            bool const changed{this->changed()};

            if (changed)
            {
                if (exists())
                {
                    // There is a reason to reopen the dataset. Try to do so.
                    try
                    {
                        _dataset = data_model::Dataset(_name);
                        _write_time = std::filesystem::last_write_time(_path);
                    }
                    catch (...)
                    {
                        // Assume the dataset is still being updated
                        _dataset.reset();
                    }
                }
                else
                {
                    // The dataset has disappeared
                    _dataset.reset();
                }

                // _cache = std::make_shared<Cache>();
            }

            return changed;
        }


        /*!
            @brief      Return whether the dataset is opened
        */
        bool Dataset::is_open() const
        {
            return _dataset.has_value();
        }


        /*!
            @brief      Return whether the dataset exists
        */
        bool Dataset::exists() const
        {
            return std::filesystem::exists(_path);
        }


        /*!
            @brief      Return whether the dataset has changed

            A dataset is changed when:
            - It was opened, but now it does not exist anymore
            - It was deleted (not opened), but now it exists again
            - It was opened, but has been updated afterwards
        */
        bool Dataset::changed() const
        {
            bool const is_open{this->is_open()};
            bool const exists{this->exists()};
            bool result{false};

            if (!is_open)
            {
                // The dataset was deleted in the past

                if (exists)
                {
                    // The dataset is available again
                    result = true;
                }
            }
            else
            {
                // The dataset was opened in the past

                if (!exists)
                {
                    // The dataset is not available anymore
                    result = true;
                }
                else
                {
                    // Assume the dataset got changed if the current write time
                    // is newer than the previous one
                    std::filesystem::file_time_type write_time{std::filesystem::last_write_time(_path)};

                    result = write_time > _write_time;
                }
            }

            return result;
        }

    }  // namespace view
}  // namespace lue

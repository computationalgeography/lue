#include "dataset_to_visualize.hpp"


namespace lue {
namespace utility {

/*!
    @brief      Construct an instance
    @param      .
    @return     .
    @exception  .

    A dataset named @a name must exist.
*/
DatasetToVisualize::DatasetToVisualize(
    std::string const& name):

    _name{name},
    _dataset{std::make_optional<Dataset>(name)},
    _path{boost::filesystem::canonical(_dataset->pathname())},
    _write_time{boost::filesystem::last_write_time(_path)}

{
}


/*!
    @brief      Return the name passed into the constructor
*/
std::string const& DatasetToVisualize::name() const
{
    return _name;
}


/*!
    @brief      Return the layered dataset
    @warning    A dataset must be opened
*/
Dataset const& DatasetToVisualize::dataset() const
{
    assert(_dataset);
    return *_dataset;
}


/*!
    @brief      Return the normalized pathname
*/
boost::filesystem::path const& DatasetToVisualize::path() const
{
    return _path;
}


std::string DatasetToVisualize::pathname() const
{
    return _path.string();
}


std::string DatasetToVisualize::filename() const
{
    return _path.filename().string();
}


/*!
    @brief      Handle removal, reappearance or update of dataset
*/
void DatasetToVisualize::rescan()
{
    if(changed()) {
        if(exists()) {
            _dataset = Dataset(_name);
            _write_time = boost::filesystem::last_write_time(_path);
        }
        else {
            _dataset.reset();
        }
    }
}


/*!
    @brief      Return whether the dataset is opened
*/
bool DatasetToVisualize::is_open() const
{
    return _dataset.has_value();
}


/*!
    @brief      Return whether the dataset exists
*/
bool DatasetToVisualize::exists() const
{
    return boost::filesystem::exists(_path);
}


/*!
    @brief      Return whether the dataset has changed

    A dataset is changed when:
    - It was opened, but now it does not exist anymore
    - It was deleted (not opened), but now it exists again
    - It was opened, but has been updated afterwards
*/
bool DatasetToVisualize::changed() const
{
    bool const is_open{this->is_open()};
    bool const exists{this->exists()};
    bool result{false};

    if(!is_open) {
        // The dataset was deleted in the past

        if(exists) {
            // The dataset is available again
            result = true;
        }
    }
    else {
        // The dataset was opened in the past

        if(!exists) {
            // The dataset is not available anymore
            result = true;
        }
        else {
            // Assume the dataset got changed if the current write time
            // is newer than the previous one
            std::time_t write_time{boost::filesystem::last_write_time(_path)};

            result = write_time > _write_time;
        }
    }

    return result;
}

}  // namespace utility
}  // namespace lue

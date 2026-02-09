#include <utility>

#include "lue/hdf5/issue.hpp"


namespace lue::hdf5 {

    /*!
        @brief      Construct issue based on @a id of HDF5 object and @a message
    */
    Issue::Issue(Identifier id, std::string message):

        _id{std::move(id)},
        _message{std::move(message)}

    {
    }


    /*!
        @brief      Return id of HDF5 object related to the issue
    */
    auto Issue::id() const -> Identifier const&
    {
        return _id;
    }


    /*!
        @brief      Return message describing the issue
    */
    auto Issue::message() const -> std::string const&
    {
        return _message;
    }

}  // namespace lue::hdf5

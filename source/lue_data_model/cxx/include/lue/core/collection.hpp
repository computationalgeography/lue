#pragma once
#include "lue/hdf5/group.hpp"
#include <fmt/format.h>
#include <cassert>
#include <map>
#include <stdexcept>
#include <vector>


namespace lue {

/*!
    @brief      A collection of HDF5 objects
    @sa         create_collection(hdf5::Identifier const&, std::string const&)

    The collection is modeled using an HDF5 group.
*/
template<
    typename T>
class Collection:
    public hdf5::Group
{

public:

    using iterator = typename std::map<std::string, T>::iterator;

    using const_iterator = typename std::map<std::string, T>::const_iterator;

                   Collection          (hdf5::Group& parent,
                                        std::string const& name);

    explicit       Collection          (Group&& group);

                   Collection          (Collection const&)=delete;

                   Collection          (Collection&&)=default;

                   ~Collection         ()=default;

    Collection&    operator=           (Collection const&)=delete;

    Collection&    operator=           (Collection&&)=default;

    bool           empty               () const;

    size_t         size                () const;

    std::vector<std::string>
                   names               () const;

    bool           contains            (std::string const& name) const;

    T const&       operator[]          (std::string const& name) const;

    T&             operator[]          (std::string const& name);

    T&             add                 (std::string const& name,
                                        T&& item);

    iterator       begin               ();

    const_iterator begin               () const;

    iterator       end                 ();

    const_iterator end                 () const;

private:

    std::vector<std::string>
                   item_names          () const;

    std::map<std::string, T> _items;

};


template<
    typename T>
Collection<T>      create_collection   (hdf5::Group const& group);


/*!
    @brief      Constructor
    @param      location Identifier of file or group of group to open
    @exception  std::runtime_error In case the group cannot be opened
*/
template<
    typename T>
inline Collection<T>::Collection(
    hdf5::Group& parent,
    std::string const& name)

    : hdf5::Group{parent, name},
      _items{}

{
    if(!id().is_valid()) {
        throw std::runtime_error(fmt::format(
                "Collection {} cannot be opened",
                name
            ));
    }

    // Open items, if available.
    for(auto const& name_: item_names()) {
        _items.insert(std::make_pair(name_, T{*this, name_}));
    }
}


template<
    typename T>
inline Collection<T>::Collection(
    hdf5::Group&& group)

    : hdf5::Group{std::forward<hdf5::Group>(group)},
      _items{}

{
    // Open items, if available.
    for(auto const& name_: item_names()) {
        _items.insert(std::make_pair(name_, T{*this, name_}));
    }
}


template<
    typename T>
inline bool Collection<T>::empty() const
{
    return _items.empty();
}


template<
    typename T>
inline size_t Collection<T>::size() const
{
    return _items.size();
}


/*!
    @brief      Return names of items
*/
template<
    typename T>
inline std::vector<std::string> Collection<T>::names() const
{
    std::vector<std::string> names;
    names.reserve(_items.size());

    for(auto const& pair: _items) {
        names.emplace_back(pair.first);
    }

    return names;
}


/*!
    @brief      Add an item to the collection
    @param      name Name of item to add
    @param      item Item to add
    @return     A reference to the added item
*/
template<
    typename T>
inline T& Collection<T>::add(
    std::string const& name,
    T&& item)
{
    assert(!contains(name));

    auto pair = _items.insert(std::make_pair(name, T{std::forward<T>(item)}));

    return (*pair.first).second;
}


/*!
    @brief      Return whether or not the collection contains an item with
                the name passed in
    @param      name Name of item
*/
template<
    typename T>
inline bool Collection<T>::contains(
    std::string const& name) const
{
    return _items.find(name) != _items.end();
}


/*!
    @brief      Return item
    @param      name Name of item
    @exception  std::runtime_error In case the item does not exist
*/
template<
    typename T>
inline T const& Collection<T>::operator[](
    std::string const& name) const
{
    auto iterator = _items.find(name);

    if(iterator == _items.end()) {
        throw std::runtime_error(fmt::format(
                "Item {} does not exist",
                name
            ));
    }

    return (*iterator).second;
}


/*!
    @brief      Return item
    @param      name Name of item
    @exception  std::runtime_error In case the item does not exist
*/
template<
    typename T>
inline T& Collection<T>::operator[](
    std::string const& name)
{
    auto iterator = _items.find(name);

    if(iterator == _items.end()) {
        throw std::runtime_error(fmt::format(
                "Item {} does not exist",
                name
            ));
    }

    return (*iterator).second;
}


template<
    typename T>
inline typename Collection<T>::iterator Collection<T>::begin()
{
    return _items.begin();
}


template<
    typename T>
inline typename Collection<T>::const_iterator Collection<T>::begin() const
{
    return _items.begin();
}


template<
    typename T>
inline typename Collection<T>::iterator Collection<T>::end()
{
    return _items.end();
}


template<
    typename T>
inline typename Collection<T>::const_iterator Collection<T>::end() const
{
    return _items.end();
}


template<
    typename T>
inline std::vector<std::string> Collection<T>::item_names() const
{
    return group_names();
}


inline bool collection_exists(
    hdf5::Group& parent,
    std::string const& name)
{
    return hdf5::group_exists(parent, name);
}


template<
    typename T>
inline Collection<T> create_collection(
    hdf5::Group& parent,
    std::string const& name)
{
    return Collection<T>{hdf5::create_group(parent, name)};
}

} // namespace lue

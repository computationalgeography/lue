#pragma once
#include "lue/hdf5/group.hpp"
#include <algorithm>
#include <cassert>
#include <format>
#include <map>
#include <stdexcept>
#include <vector>


namespace lue::data_model {

    /*!
        @brief      A collection of HDF5 objects
        @sa         create_collection(hdf5::Identifier const&, std::string const&)

        The collection is modeled using an HDF5 group.
    */
    template<typename T>
    class Collection: public hdf5::Group
    {

        public:

            using Element = T;

            using iterator = typename std::map<std::string, T>::iterator;

            using const_iterator = typename std::map<std::string, T>::const_iterator;

            template<typename... Argument>
            Collection(hdf5::Group const& parent, std::string const& name, Argument const&... argument);

            template<typename... Argument>
            explicit Collection(hdf5::Group&& group, Argument const&... argument);

            Collection(Collection const& other) = default;

            Collection(Collection&& other) noexcept = default;

            ~Collection() override = default;

            auto operator=(Collection const& other) -> Collection& = default;

            auto operator=(Collection&& other) noexcept -> Collection& = default;

            auto empty() const -> bool;

            auto size() const -> size_t;

            auto names() const -> std::vector<std::string>;

            auto contains(std::string const& name) const -> bool;

            auto operator[](std::string const& name) const -> T const&;

            auto operator[](std::string const& name) -> T&;

            auto add(std::string const& name, T&& item) -> T&;

            auto begin() -> iterator;

            auto begin() const -> const_iterator;

            auto end() -> iterator;

            auto end() const -> const_iterator;

        private:

            auto item_names() const -> std::vector<std::string>;

            std::map<std::string, T> _items;
    };


    template<typename T>
    auto create_collection(hdf5::Group const& group) -> Collection<T>;


    /*!
        @brief      Constructor
        @param      location Identifier of file or group of group to open
        @exception  std::runtime_error In case the group cannot be opened
    */
    template<typename T>
    template<typename... Argument>
    inline Collection<T>::Collection(
        hdf5::Group const& parent, std::string const& name, Argument const&... argument):

        hdf5::Group{parent, name},
        _items{}

    {
        if (!id().is_valid())
        {
            throw std::runtime_error(std::format("Collection {} cannot be opened", name));
        }

        // Open items, if available.
        for (auto const& name_ : item_names())
        {
            _items.insert(std::make_pair(name_, T{*this, name_, argument...}));
        }
    }


    template<typename T>
    template<typename... Argument>
    inline Collection<T>::Collection(hdf5::Group&& group, Argument const&... argument):

        hdf5::Group{std::move(group)},
        _items{}

    {
        // Open items, if available.
        for (auto const& name_ : item_names())
        {
            _items.insert(std::make_pair(name_, T{*this, name_, argument...}));
        }
    }


    template<typename T>
    inline auto Collection<T>::empty() const -> bool
    {
        return _items.empty();
    }


    template<typename T>
    inline auto Collection<T>::size() const -> size_t
    {
        return _items.size();
    }


    /*!
        @brief      Return names of items
    */
    template<typename T>
    inline auto Collection<T>::names() const -> std::vector<std::string>
    {
        std::vector<std::string> result(_items.size());

        std::transform(
            _items.begin(),
            _items.end(),
            result.begin(),

            [](auto const& pair) -> auto { return pair.first; }

        );

        return result;
    }


    /*!
        @brief      Add an item to the collection
        @param      name Name of item to add
        @param      item Item to add
        @return     A reference to the added item
    */
    template<typename T>
    inline auto Collection<T>::add(std::string const& name, T&& item) -> T&
    {
        assert(!contains(name));

        auto pair = _items.insert(std::make_pair(name, T{std::move(item)}));

        return (*pair.first).second;
    }


    /*!
        @brief      Return whether or not the collection contains an item with the name passed in
        @param      name Name of item
    */
    template<typename T>
    inline auto Collection<T>::contains(std::string const& name) const -> bool
    {
        return _items.find(name) != _items.end();
    }


    /*!
        @brief      Return item
        @param      name Name of item
        @exception  std::runtime_error In case the item does not exist
    */
    template<typename T>
    inline auto Collection<T>::operator[](std::string const& name) const -> T const&
    {
        auto iterator = _items.find(name);

        if (iterator == _items.end())
        {
            throw std::runtime_error(std::format("Item {} does not exist", name));
        }

        return (*iterator).second;
    }


    /*!
        @brief      Return item
        @param      name Name of item
        @exception  std::runtime_error In case the item does not exist
    */
    template<typename T>
    inline auto Collection<T>::operator[](std::string const& name) -> T&
    {
        auto iterator = _items.find(name);

        if (iterator == _items.end())
        {
            throw std::runtime_error(std::format("Item {} does not exist", name));
        }

        return (*iterator).second;
    }


    template<typename T>
    inline auto Collection<T>::begin() -> typename Collection<T>::iterator
    {
        return _items.begin();
    }


    template<typename T>
    inline auto Collection<T>::begin() const -> typename Collection<T>::const_iterator
    {
        return _items.begin();
    }


    template<typename T>
    inline auto Collection<T>::end() -> typename Collection<T>::iterator
    {
        return _items.end();
    }


    template<typename T>
    inline auto Collection<T>::end() const -> typename Collection<T>::const_iterator
    {
        return _items.end();
    }


    template<typename T>
    inline auto Collection<T>::item_names() const -> std::vector<std::string>
    {
        return group_names();
    }


    inline auto collection_exists(hdf5::Group const& parent, std::string const& name) -> bool
    {
        return hdf5::group_exists(parent, name);
    }


    template<typename T>
    inline auto create_collection(hdf5::Group& parent, std::string const& name) -> Collection<T>
    {
        return Collection<T>{hdf5::create_group(parent, name)};
    }

}  // namespace lue::data_model

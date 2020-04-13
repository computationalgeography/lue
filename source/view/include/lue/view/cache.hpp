#pragma once
#include "lue/hdf5/identifier.hpp"
#include <any>
#include <cassert>
#include <functional>
#include <map>

#include <iostream>


namespace lue {
namespace view {

class Cache
{

public:

    using Key = hdf5::Identifier;

                   Cache               ()=default;

                   Cache               (Cache const&)=default;

                   Cache               (Cache&&)=default;

                   ~Cache              ()=default;

    Cache&         operator=           (Cache const&)=default;

    Cache&         operator=           (Cache&&)=default;

    bool           contains            (Key const& key) const;

    template<
        typename Object>
    void insert(
        Key const& key,
        Object const& object)
    {
        assert(!contains(key));

        _objects[key] = std::make_any<Object>(object);

        assert(contains(key));
        assert(_objects[key].has_value());
    }

    template<
        typename Object>
    Object& retrieve(
        Key const& key)
    {
        assert(contains(key));
        assert(_objects[key].has_value());

        std::cout << "->-> " << &_objects.at(key) << std::endl;

        return std::any_cast<Object&>(_objects.at(key));
    }

private:

    std::map<Key, std::any> _objects;

};

}  // namespace view
}  // namespace lue

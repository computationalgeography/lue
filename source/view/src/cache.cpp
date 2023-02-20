#include "lue/view/cache.hpp"


namespace lue {
    namespace view {

        bool Cache::contains(Key const& key) const
        {
            return _objects.find(key) != _objects.end();
        }

    }  // namespace view
}  // namespace lue
